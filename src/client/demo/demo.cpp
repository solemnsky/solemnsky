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

void Demo::handle(const sf::Event &event) {
  skyButton.handle(event);
  graphicsButton.handle(event);
  physicsButton.handle(event);
}

void Demo::signalRead() {
  if (skyButton.clickSignal)
    next = std::make_unique<LinkBack>(std::make_unique<SkyDemo>());
  if (graphicsButton.clickSignal)
    next = std::make_unique<LinkBack>(std::make_unique<GraphicsDemo>());
  if (physicsButton.clickSignal)
    next = std::make_unique<LinkBack>(std::make_unique<PhysDemo>());
}

void Demo::signalClear() {
  skyButton.signalClear();
  graphicsButton.signalClear();
  physicsButton.signalClear();
}

ui::Button::Style backButtonStyle() {
  ui::Button::Style style;
  style.alpha = 0.5;
  return style;
}

LinkBack::LinkBack(std::shared_ptr<ui::Control> ctrl) :
    underlying(ctrl), backButton({100, 50}, "go back", backButtonStyle()) {
}

void LinkBack::tick(float delta) {
  underlying->tick(delta);
  backButton.tick(delta);
}

void LinkBack::render(ui::Frame &f) {
  underlying->render(f);
  backButton.render(f);
}

void LinkBack::handle(const sf::Event &event) {
  underlying->handle(event);
  backButton.handle(event);
}

void LinkBack::signalRead() {
  underlying->signalRead();

  if (backButton.clickSignal) next = std::make_unique<Demo>();
  if (underlying->next) next = std::move(underlying->next);
}

void LinkBack::signalClear() {
  underlying->signalClear();
}

