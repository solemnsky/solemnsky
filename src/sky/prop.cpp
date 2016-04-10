#include "prop.h"
#include "sky.h"

namespace sky {

Prop::Prop(Sky &parent, const sf::Vector2f &pos, const sf::Vector2f &vel) :
    parent(parent), physics(parent.physics),
    lifeTime(0),
    body(physics.rectBody({10, 10})),
    physical(pos, vel, 0, 0) {
  physical.hardWriteToBody(physics, body);
}

Prop::Prop(Prop &&prop) :
    parent(prop.parent), physics(prop.physics),
    lifeTime(prop.lifeTime),
    body(prop.body),
    physical(prop.physical) {
  prop.body = nullptr;
}

Prop::~Prop() {
  if (body) {
    physics.clrBody(body);
  }
}

void Prop::writeToBody() {
  physical.writeToBody(physics, body);
}

void Prop::readFromBody() {
  physical.readFromBody(physics, body);
}

void Prop::tick(const float delta) {
  lifeTime += delta;
}

}