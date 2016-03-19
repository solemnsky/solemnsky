#include "splash.h"
#include "client/util/resources.h"

namespace ui {

namespace detail {
void SplashScreen::tick(float delta) {
  if (screenDrawn) {
    loadResources(); // this might call more than once but it doesn't reload
    if (!next) next = std::move(afterLoading());
  }
}

void SplashScreen::render(ui::Frame &f) {
  auto p = TextProperties::normal;
  p.size = 50;
  p.font = font;
  f.drawText({800, 450}, {"loading resources..."}, p);
  screenDrawn = true;
}

bool SplashScreen::handle(const sf::Event &event) { }
}

}
