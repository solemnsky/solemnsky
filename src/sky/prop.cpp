#include "prop.h"
#include "sky.h"

namespace sky {

Prop::Prop(SkyHolder &parent, const sf::Vector2f &pos, const sf::Vector2f &vel)
    :
    parent(parent),
    physics(parent.physics.get()),
    body(physics.createBody(physics.rectShape({10, 10}),
                            BodyTag::PropTag(*this))),
    lifeTime(0),
    physical(pos, vel, 0, 0) {
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
  lifeTime += delta;
}

}
