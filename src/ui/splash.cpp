#include "splash.h"
#include "client/elements/style.h"
#include "util/methods.h"

namespace ui {

namespace detail {

SplashScreen::SplashScreen(std::function<std::unique_ptr<Control>()> afterLoading)
    : loadingText(50, {},
                  ui::HorizontalAlign::Right, ui::VerticalAlign::Top,
                  ResID::Font),
      afterLoading(afterLoading) {
  appLog("starting...");
}

void SplashScreen::tick(float delta) {
  if (screenDrawn) {
    loadResources(); // guards against repeated loading
    if (!next) next = std::move(afterLoading());
  }
}

void SplashScreen::render(ui::Frame &f) {
  loadSplashResources();
  f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
  f.drawText({800, 450}, "loading resources...",
             sf::Color::White, loadingText);
  screenDrawn = true;
}

bool SplashScreen::handle(const sf::Event &event) { }
}

}
