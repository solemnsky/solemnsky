#include "physdemo.h"

sky::Physics::Settings mkSettings() {
  sky::Physics::Settings settings{};
  settings.distanceScalar = 100; // etc
  return settings;
}

PhysDemo::PhysDemo() :
    physics{{1600, 900}, mkSettings()} {
  rectBody = physics.createBody({800, 450}, true);
  physics.addRectFixture(rectBody, {20, 20});

  circleBody = physics.createBody({800, 500}, true);
  physics.addCircleFixture(circleBody, 20);

  reset();
}

void PhysDemo::reset() {
  rectBody->SetTransform(physics.toPhysVec({800, 450}), 20);
  circleBody->SetTransform(physics.toPhysVec({800, 500}), 0);
  rectBody->SetLinearVelocity({0, 0});
  circleBody->SetLinearVelocity({0, 0});
  rectBody->SetAngularVelocity(0);
  circleBody->SetAngularVelocity(0);
}

void PhysDemo::tick(float delta) {
  physics.world.ClearForces();
  physics.tick(delta);
}

void PhysDemo::render(ui::Frame &f) {
  f.withTransform(
      sf::Transform()
          .translate(physics.toGameVec(rectBody->GetPosition()))
          .rotate(rectBody->GetAngle()),
      [&]() { f.drawRect({-10, -10, 20, 20}, sf::Color::Red); });

  f.drawCircle(physics.toGameVec(circleBody->GetPosition()), 20,
               sf::Color::Red);
}

void PhysDemo::handle(sf::Event event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::F) reset();
  }
}

