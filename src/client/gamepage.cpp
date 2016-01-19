#include "gamepage.h"

void GamePage::tick(float delta) {

}

void GamePage::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"game page", "where the user can play a game"}, 40);
}

void GamePage::handle(const sf::Event &event) {

}
