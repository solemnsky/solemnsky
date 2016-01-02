#include "demo.h"
#include "enginedemo.h"

void Demo::tick(float delta) {
  button.tick(delta);
}

void Demo::render(ui::Frame &f) {
  button.render(f);
}

void Demo::handle(sf::Event event) {
  button.handle(event);
  if (button.isClicked) next = std::make_shared<EngineDemo>();
}
