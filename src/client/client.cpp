#include "client.h"

Client::Client() : sky({1600, 900}) {

}

void Client::tick(float delta) { }

void Client::render(ui::Frame &f) {
  f.drawText({800, 450}, {"we still have to write this part, one sec"});
}

void Client::handle(const sf::Event &event) { }

