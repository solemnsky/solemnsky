#include "tutorial.h"

void Tutorial::tick(float delta) {

}

void Tutorial::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"game page", "where the user can play a game"}, 40);
}

void Tutorial::handle(const sf::Event &event) {

}
