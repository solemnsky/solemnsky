#include "physdemo.h"

sky::Physics::Settings mkSettings() {
  sky::Physics::Settings settings{};
  return settings;
}

PhysDemo::PhysDemo() :
    physics{{1600, 900}, mkSettings()} {
  b2Body *body;
  b2Fixture *fix;

  for (int i = 0; i < 1; ++i) {
    body = physics.createBody({0, 0}, true);
    fix = physics.addRectFixture(body, {40, 40});
    fix->SetRestitution(0.1);

    b2MassData data;
    data.center = {0, 0};
    data.mass = 100;
    data.I = 0.01;
    body->SetMassData(&data);

    bodies.push_back(body);
  }

  reset();
}

void PhysDemo::reset() {
  for (auto &body : bodies) {
    body->SetTransform(physics.toPhysVec({800, 450}), 45);
    body->SetAngularVelocity(0);
  }
}

void PhysDemo::tick(float delta) {
  for (auto &body : bodies) physics.setRotVel(body, 50);
  physics.tick(delta);
}

void PhysDemo::render(ui::Frame &f) {
  const std::vector<std::string> help{
      "escape: reset",
      "r: rotate (reach target rotation, preserve velocity)",
      "s: stop (reach target rest)"
  };

//  f.drawText({300, 50}, help, 40);

  f.window.setTitle("physics demo");

  for (auto &body : bodies) {
    f.withTransform(
        sf::Transform().translate(physics.toGameVec(body->GetPosition())).
            rotate(body->GetAngle()),
        [&]() {
          f.drawRect({40, 40}, sf::Color::White);
          f.drawRect({38, 38}, sf::Color::Red);
        });
  }
  f.drawText({1300, 700}, {
      "boundary: " + std::to_string(physics.toPhysDistance(1600)),
      "size: " + std::to_string(physics.toPhysDistance(40)),
      "inertia: " + std::to_string(bodies[0]->GetInertia()),
      "mass: " + std::to_string(bodies[0]->GetMass())
  }, 30);
}

void PhysDemo::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::Escape) reset();
    if (event.key.code == sf::Keyboard::Key::R) {
      for (auto &body : bodies) physics.setRotVel(body, 50);
    }
    if (event.key.code == sf::Keyboard::Key::S) {
      for (auto &body : bodies) {
        physics.setRotVel(body, 0);
        physics.setVel(body, {0, 0});
      }
    }
  }
}

