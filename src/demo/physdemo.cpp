#include "physdemo.h"

PhysDemo::PhysDemo() : world({0, 10}) {
  b2Body *body;

  b2BodyDef floorDef;
  floorDef.type = b2_staticBody;
  b2PolygonShape floorShape;
  floorShape.SetAsBox(8, 0.1);
  body = world.CreateBody(&floorDef);
  body->CreateFixture(&floorShape, 0);
  body->SetTransform({8, 9}, 0);

  for (int i = 0; i < 10; ++i) {
    b2BodyDef def;
    def.fixedRotation = false;
    def.type = b2_dynamicBody;

    b2PolygonShape shape;
    shape.SetAsBox(0.2, 0.2);
    body = world.CreateBody(&def);
    body->CreateFixture(&shape, 10.0f);
    bodies.push_back(body);
  }

  reset();
}

void PhysDemo::reset() {
  int i = 1;
  for (auto &body : bodies) {
    body->SetTransform({i, 4.5}, 45);
    body->SetLinearVelocity({0, 0});
    body->SetAngularVelocity(0);
    i++;
  }
}

void PhysDemo::tick(float delta) {
  world.Step(delta, 8, 3);
  for (auto &body : bodies) {
    body->ApplyAngularImpulse((2 - body->GetAngularVelocity()) *
                              body->GetInertia(), true);
  }
}

void PhysDemo::render(ui::Frame &f) {
  f.drawText({500, 50}, {"if this screws up, I'm going home"}, 30);
  for (auto &body : bodies) {
    const b2Vec2 &pos = body->GetPosition();
    f.withTransform(
        sf::Transform().translate(pos.x * 100, pos.y * 100).
            rotate((float) (body->GetAngle() * (180 / 3.1416))),
        [&]() {
          f.drawRect({40, 40}, sf::Color::White);
          f.drawRect({38, 38}, sf::Color::Red);
        });
  }

  f.drawText({1300, 700}, {
//      "boundary: " + std::to_string(physics.toPhysDistance(1600)),
//      "size: " + std::to_string(physics.toPhysDistance(40)),
//      "inertia: " + std::to_string(bodies[0]->GetInertia()),
//      "mass: " + std::to_string(bodies[0]->GetMass())
  }, 30);
}

void PhysDemo::handle(const sf::Event &event) {
  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Key::Escape) reset();
    if (event.key.code == sf::Keyboard::Key::R) {
//      for (auto &body : bodies) physics.setRotVel(body, 50);
    }
    if (event.key.code == sf::Keyboard::Key::S) {
//      for (auto &body : bodies) {
//        physics.setRotVel(body, 0);
//        physics.setVel(body, {0, 0});
//      }
    }
  }
}

