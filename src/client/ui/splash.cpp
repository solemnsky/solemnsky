#include "splash.h"
#include "client/elements/style.h"
#include "util/methods.h"

namespace ui {

namespace detail {

const sf::Font &SplashScreen::loadFont() {
  font.loadFromFile(recordOf(Res::Font).realPath());
  return font;
}

SplashScreen::SplashScreen(std::function<std::unique_ptr<Control>()> afterLoading)
    : loadingText(50, {},
                  ui::HorizontalAlign::Right, ui::VerticalAlign::Top,
                  loadFont()),
      afterLoading(afterLoading) {
  appLog("starting...");
}

void SplashScreen::tick(float delta) {
  if (screenDrawn) {
    loadResources(); // load the resources from files
    style.emplace(); // emplace stylesheet

    if (!next) next = std::move(afterLoading());
  }
}

void SplashScreen::render(ui::Frame &f) {
  f.drawText({800, 450}, "loading resources...",
             sf::Color::White, loadingText);
  screenDrawn = true;
}

bool SplashScreen::handle(const sf::Event &event) { }
}

}
