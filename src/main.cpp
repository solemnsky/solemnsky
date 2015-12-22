#include <iostream>
#include "ctrl/ctrl.h"
#include <Box2D/Box2D.h>

class DemoControl : public Control {
private:
    b2World world;
    b2Body* body;

    sf::Vector2f mousePos{};
    float time = 0;
    void renderPlanet(Frame &f, sf::Vector2f center);

public:
    DemoControl();

    void tick(float delta) override;
    void render(Frame &f) override;
    void handle(sf::Event event) override;
    void renderSystem(Frame &f);
};

void DemoControl::renderPlanet(Frame &f, sf::Vector2f center) {
    f.pushTransform(sf::Transform().translate(center).rotate(time * 30));

    sf::Vector2f pos(0, 0), offset(27, 0);

    f.drawCircle(pos, 20, sf::Color(0, 255, 0, 255));
    f.drawCircle(pos + offset, 7, sf::Color(0, 255, 0, 255));

    f.popTransform();
}

void DemoControl::renderSystem(Frame &f) {
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

void DemoControl::render(Frame &f) {
//    renderPlanet(f, FromBox2d::vector(body->GetPosition()));
    renderSystem(f);
}

void DemoControl::handle(sf::Event event) {
    if (event.type == sf::Event::EventType::MouseMoved) {
        mousePos = sf::Vector2f(event.mouseMove.x, event.mouseMove.y);
    }
}

DemoControl::DemoControl() : world(b2Vec2(0.0f, -10.0f)){
    // groundBody
    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, -10.0f);
    b2Body* groundBody = world.CreateBody(&groundBodyDef);

    // groundBodyShape
    b2PolygonShape groundBodyShape;
    groundBodyShape.SetAsBox(50.0f, 10.0f);
    groundBody->CreateFixture(&groundBodyShape, 0.0f);

    // body
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(0.0f, 4.0f);
    b2Body* body = world.CreateBody(&bodyDef);

    // bodyFixture
    b2PolygonShape bodyShape;
    bodyShape.SetAsBox(1.0f, 1.0f);
    b2FixtureDef bodyFixture;
    bodyFixture.shape = &bodyShape;
    bodyFixture.density = 1.0f;
    bodyFixture.friction = 0.3f;
    body->CreateFixture(&bodyFixture);

    float32 timeStep = 1.0f / 60.0f;
}

void DemoControl::tick(float delta) {
    time += delta;
    world.Step(delta, 6, 2);
}

int main() {
    DemoControl face{};
    runSFML(face);
    return 0;
}