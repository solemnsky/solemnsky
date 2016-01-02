#include "graphicsdemo.h"

void GraphicsDemo::renderPlanet(ui::Frame &f, sf::Vector2f center) {
  f.pushTransform(sf::Transform().translate(center).rotate(time * 30));

  sf::Vector2f pos(0, 0), offset(27, 0);

  f.drawCircle(pos, 20, sf::Color(0, 255, 0, 255));
  f.drawCircle(pos + offset, 7, sf::Color(0, 255, 0, 255));

  f.popTransform();
}

void GraphicsDemo::renderSystem(ui::Frame &f) {
  f.pushTransform(sf::Transform().translate(mousePos).rotate(-time * 30));

  sf::Vector2f offset(40, -40), offset2(40, 40), pos;

  pos = sf::Vector2f(0, 0);
  for (int i = 1; i < 20; ++i) {
    renderPlanet(f, pos);
    pos += offset;
  }

  pos = sf::Vector2f(0, 0);
  for (int i = 1; i < 20; ++i) {
    renderPlanet(f, pos);
    pos += offset2;
  }

  pos = sf::Vector2f(0, 0);
  for (int i = 1; i < 20; ++i) {
    renderPlanet(f, pos);
    pos -= offset2;
  }

  pos = sf::Vector2f(0, 0);
  for (int i = 1; i < 20; ++i) {
    renderPlanet(f, pos);
    pos -= offset;
  }

  f.popTransform();
}

void GraphicsDemo::tick(float delta) {
  time += delta;
}

void GraphicsDemo::render(ui::Frame &f) {
  renderSystem(f);
}

void GraphicsDemo::handle(sf::Event event) {
  if (event.type == sf::Event::EventType::MouseMoved) {
    mousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
  }
}

