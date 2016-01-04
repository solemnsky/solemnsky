#include "demo.h"
#include "skydemo.h"
#include "graphicsdemo.h"
#include "physdemo.h"

void Demo::tick(float delta) {
  skyButton.tick(delta);
  graphicsButton.tick(delta);
  physicsButton.tick(delta);
}

void Demo::render(ui::Frame &f) {
  skyButton.render(f);
  graphicsButton.render(f);
  physicsButton.render(f);
}

void Demo::handle(sf::Event event) {
  skyButton.handle(event);
  graphicsButton.handle(event);
  physicsButton.handle(event);
}

void Demo::update() {
  if (skyButton.isClicked)
    next = std::make_shared<LinkBack>(std::make_shared<SkyDemo>());
  if (graphicsButton.isClicked)
    next = std::make_shared<LinkBack>(std::make_shared<GraphicsDemo>());
  if (physicsButton.isClicked)
    next = std::make_shared<LinkBack>(std::make_shared<PhysDemo>());
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
  if (backButton.isClicked) next = std::make_shared<Demo>();
  if (underlying->next) next = underlying->next;
}
