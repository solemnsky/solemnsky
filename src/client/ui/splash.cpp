#include "splash.h"
#include "client/elements/style.h"

namespace ui {

namespace detail {
void SplashScreen::tick(float delta) {
  if (screenDrawn) {
    loadResources(); // this might call more than once but it doesn't reload
    if (!next) next = std::move(afterLoading());
  }
}

void SplashScreen::render(ui::Frame &f) {
  f.drawText({800, 450}, "loading resources...",
             sf::Color::White, style.splash.loadingText);
  screenDrawn = true;
}

bool SplashScreen::handle(const sf::Event &event) { }
}

}
