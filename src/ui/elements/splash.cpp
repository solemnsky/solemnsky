#include "splash.h"
#include "base/resources.h"

namespace ui {

namespace detail {
void SplashScreen::tick(float delta) {
  if (screenDrawn) {
    loadResources(); // this might call more than once but it doesn't reload
    if (afterLoading) next = std::move(afterLoading);
  }
}

void SplashScreen::render(ui::Frame &f) {
  f.drawText({800, 450}, {"loading resources..."}, 50, {255, 255, 255}, font);
  screenDrawn = true;
}

void SplashScreen::handle(const sf::Event &event) { }
}

}
