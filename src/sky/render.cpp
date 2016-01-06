#include <ui/frame.h>
#include "render.h"

namespace sky {

RenderMan::RenderMan(Sky *sky) :
    sky(sky),
    sheet(Res::PlayerSheet) { }

void RenderMan::render(ui::Frame &f, sf::Vector2f pos) {
  if (sky::Plane *plane = sky->getPlane(0)) {
    if (plane->state) {
      sheet.drawIndex(f, plane->state->pos, sf::Vector2f(200, 200), 7);
    } else {
      f.drawText(sf::Vector2f(800, 200), "You're dead, so sorry ...", 40,
                 sf::Color::Black);
    }
  }
}

void RenderMan::tick(float delta) {

}
}

