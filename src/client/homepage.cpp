#include "homepage.h"
#include "base/resources.h"

HomePage::HomePage(ClientState *state) : state(state) { }

void HomePage::tick(float delta) {

}

void HomePage::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
  f.drawText({800, 600},
             {"home page",
              "page where the user receives the comforts of home",
              "uptime: " + std::to_string((int) (state->uptime * 1000)) + "ms"},
             40);
}

void HomePage::handle(const sf::Event &event) {

}
