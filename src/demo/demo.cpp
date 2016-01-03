#include "demo.h"
#include "skydemo.h"
#include "graphicsdemo.h"

void Demo::tick(float delta) {
  skyButton.tick(delta);
  graphicsButton.tick(delta);
}

void Demo::render(ui::Frame &f) {
  skyButton.render(f);
  graphicsButton.render(f);
}

void Demo::handle(sf::Event event) {
  skyButton.handle(event);
  graphicsButton.handle(event);
}

void Demo::update() {
  if (skyButton.isClicked)
    next = std::make_shared<LinkBack>(std::make_shared<SkyDemo>());
  if (graphicsButton.isClicked) next = std::make_shared<LinkBack>(std::make_shared<GraphicsDemo>());
}

LinkBack::LinkBack(std::shared_ptr<ui::Control> ctrl) : underlying(ctrl) { }

void LinkBack::tick(float delta) {
  underlying->tick(delta);
  backButton.tick(delta);
}

void LinkBack::render(ui::Frame &f) {
  underlying->render(f);
  backButton.render(f);
}

void LinkBack::handle(sf::Event event) {
  underlying->handle(event);
  backButton.handle(event);
}

void LinkBack::update() {
  if (underlying->next) {
    next = underlying->next;
    return;
  }
  if (backButton.isClicked) {
    next = std::make_shared<Demo>();
  }
}
