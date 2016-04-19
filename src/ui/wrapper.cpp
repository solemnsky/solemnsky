#include "wrapper.h"
#include "client/elements/style.h"

namespace ui {

namespace detail {

ExecWrapper::ExecWrapper(
    AppState &appState,
    std::function<std::unique_ptr<Control>(AppState &)>
    mainCtrlCtor) :

    Control(appState),

    animBegin(appState.time),
    drewScreen(false), loadingDone(false),
    loadingText(80, {},
                ui::HorizontalAlign::Center, ui::VerticalAlign::Middle,
                ResID::Font),
    mainCtrlCtor(mainCtrlCtor) {
  loadSplashResources();
}

void ExecWrapper::poll(float delta) {
  if (!mainCtrl && drewScreen && !loadingDone) {
    loadResources();
    loadingDone = true;
  }
  ui::Control::poll(delta);
  if (mainCtrl) {
    quitting = mainCtrl->quitting;
  }
}

void ExecWrapper::tick(float delta) {
  ui::Control::tick(delta);
}

void ExecWrapper::render(ui::Frame &f) {
  drewScreen = true;

  if (!mainCtrl) {
    f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
    if (!loadingDone) {
      f.drawText({800, 450}, "loading resources...",
                 sf::Color::White, loadingText);
    } else {
      f.withAlpha(
          linearTween(0.3, 1, sineAnim(float(appState.time), 0.2)),
          [&]() {
            f.drawText({800, 450}, "press any key to begin",
                       sf::Color::White, loadingText);
          });
    }
  } else {
    const float animTime = float(appState.timeSince(animBegin));
    if (animTime < 0.5) {
      f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
      f.withAlpha(linearTween(0, 1, animTime * 2), [&]() {
        ui::Control::render(f);
      });
    } else {
      ui::Control::render(f);
    }
  }
}

bool ExecWrapper::handle(const sf::Event &event) {
  if (loadingDone && !mainCtrl) {
    if (event.type == sf::Event::KeyPressed
        or event.type == sf::Event::KeyReleased
        or event.type == sf::Event::MouseButtonPressed) {
      mainCtrl = std::move(mainCtrlCtor(appState));
      animBegin = appState.time;
      areChildren({mainCtrl.get()});
      return true;
    }
  }

  return ui::Control::handle(event);
}

}

}
