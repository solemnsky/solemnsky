#include "wrapper.h"
#include "client/elements/style.h"
#include "util/printer.h"

namespace ui {

namespace detail {

ExecWrapper::ExecWrapper(
    AppState &appState,
    std::function<std::unique_ptr<Control>(AppState &)>
    mainCtrlCtor) :

    Control(appState),

    loadingText(50, {},
                ui::HorizontalAlign::Right, ui::VerticalAlign::Top,
                ResID::Font),
    mainCtrlCtor(mainCtrlCtor) {
  loadSplashResources();
}

void ExecWrapper::poll(float) {
  if (!mainCtrl) {
    loadResources();
    mainCtrl.emplace(mainCtrlCtor(appState));
  }
}

void ExecWrapper::tick(float delta) {
  ui::Control::tick(delta);
}

void ExecWrapper::render(ui::Frame &f) {
  f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
  if (!mainCtrl) {
    f.drawText({800, 450}, "loading resources...",
               sf::Color::White, loadingText);
  } else {
    mainCtrl->render(f);
  }
}

bool ExecWrapper::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

}

}
