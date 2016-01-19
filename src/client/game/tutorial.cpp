#include "tutorial.h"

Tutorial::Tutorial(ClientState *state) :
    Game(state) {
  name = "tutorial";
}

void Tutorial::tick(float delta) {

}

void Tutorial::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"tutorial", "demonstration of play mechanics go here"}, 40);
}

void Tutorial::handle(const sf::Event &event) {

}
