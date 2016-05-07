#include "prop.h"
#include "sky.h"

namespace sky {

Prop::Prop(Physics &physics, const sf::Vector2f &pos,
           const sf::Vector2f &vel) :
    physics(physics),
    body(physics.createBody(physics.rectShape({10, 10}),
                            BodyTag::PropTag(*this))),
    physical(pos, vel, 0, 0),
    lifetime(0) {
  physical.hardWriteToBody(physics, body);
}

Prop::~Prop() {
  physics.deleteBody(body);
}

void Prop::writeToBody() {
  physical.writeToBody(physics, body);
}

void Prop::readFromBody() {
  physical.readFromBody(physics, body);
}

void Prop::tick(const float delta) {
  lifetime += delta;
}

const PhysicalState &Prop::getPhysical() const {
  return physical;
}

float Prop::getLifetime() const {
  return lifetime;
}

}
