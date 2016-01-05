#include "physdemo.h"

sky::Physics::Settings mkSettings() {
  sky::Physics::Settings settings{};
  settings.distanceScalar = 100; // etc
  return settings;
}

PhysDemo::PhysDemo() :
    physics{{1600, 900}, mkSettings()} {
  body = physics.createBody({800, 450}, true);
  physics.addRectFixture(body, {20, 20});
  body->SetLinearVelocity(physics.toPhysVec({20, 20}));
}

void PhysDemo::tick(float delta) {
  physics.tick(delta);
}

void PhysDemo::render(ui::Frame &f) {
  f.drawCircle(physics.toGameVec(body->GetPosition()), 20, sf::Color::Red);
}

void PhysDemo::handle(sf::Event event) { }

