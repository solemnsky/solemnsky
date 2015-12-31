#include "demo.h"
#include "graphicsdemo.h"

void Demo::tick(float delta) {
}

void Demo::render(Frame &f) {

}

void Demo::handle(sf::Event event) {
  if (event.type == sf::Event::KeyPressed) {
    next = std::make_shared<GraphicsDemo>();
  }
}
