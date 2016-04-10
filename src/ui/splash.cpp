#include "splash.h"
#include "client/elements/style.h"
#include "util/printer.h"

namespace ui {

namespace detail {

SplashScreen::SplashScreen(AppState &appState,
                           std::function<std::unique_ptr<Control>(AppState &)>
                           afterLoading) :
    Control(appState),
    loadingText(50, {},
                ui::HorizontalAlign::Right, ui::VerticalAlign::Top,
                ResID::Font),
    textLog(appState,
            TextLog::Style(1580, 890, 0, 0, 0, 24), {10, 890}),
    progress(0),
    afterLoading(afterLoading),
    resourceThread(loadResources, (Printer &) textLog, progress) {
  areChildren({&textLog});
}

void SplashScreen::tick(float delta) {
  ui::Control::tick(delta);
  if (!next) next = std::move(afterLoading(appState));
}

void SplashScreen::render(ui::Frame &f) {
  loadSplashResources();
  f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
  f.drawText({800, 450}, "loading resources...",
             sf::Color::White, loadingText);
  ui::Control::render(f);
}

bool SplashScreen::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

}

}
