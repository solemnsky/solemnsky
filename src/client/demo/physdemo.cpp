#include "physdemo.h"

PhysDemo::PhysDemo() : physics({1600, 900}) {
  b2Body *body;
  for (int i = 0; i < 40; ++i) {
    body = physics.rectBody({40, 40});
    body->GetFixtureList()->SetFriction(0.5);
    bodies.push_back(body);
  }

  reset();
}

void PhysDemo::reset() {
  float i = 100;
  for (auto &body : bodies) {
    body->SetTransform(physics.toPhysVec({i, 450}), toRad(45));
    body->SetLinearVelocity({0, 0});
    body->SetAngularVelocity(0);
    i = cyclicClamp<float>(100, 1500, i + 100);
  }
}

void PhysDemo::tick(float delta) {
  for (auto &body : bodies) {
    physics.approachRotVel(body, (marchingLeft ^ rising) ? -720 : 720);
    body->SetGravityScale(rising ? -1 : 1);
  }
  physics.tick(delta);
}

void PhysDemo::render(ui::Frame &f) {
  f.drawText({500, 50}, {"use arrow keys to move them around"}, 30);
  for (auto &body : bodies) {
    f.withTransform(
        sf::Transform().translate(physics.toGameVec(body->GetPosition())).
            rotate(toDeg(body->GetAngle())),
        [&]() {
          f.drawRect({40, 40}, sf::Color::White);
          f.drawRect({38, 38}, sf::Color::Red);
        });
  }
}

void PhysDemo::handle(const sf::Event &event) {
  if (event.type == event.KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::Left) marchingLeft = true;
    if (event.key.code == sf::Keyboard::Key::Right) marchingLeft = false;
    if (event.key.code == sf::Keyboard::Key::Up) rising = true;
    if (event.key.code == sf::Keyboard::Key::Down) rising = false;

  }
}

