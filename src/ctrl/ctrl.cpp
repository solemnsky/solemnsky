#include <SFML/Graphics.hpp>
#include "log.h"
#include <iostream>
#include <assert.h>
#include "ctrl.h"

/**********************************************************************************************************************/
/* Frame
/**********************************************************************************************************************/

const sf::Color Frame::alphaScaleColor(const sf::Color color) {
    sf::Color newColor(color);
    newColor.a *= alphaStack.top();
    return newColor;
}

Frame::Frame(sf::RenderWindow &window) : window{window} {
    resize();
    reset();
}

void Frame::resize() {
    // set in function of window
    parentTransform = sf::Transform();
}

void Frame::reset() {
    transformStack = (decltype(transformStack)) {{parentTransform}};
    alphaStack = (decltype(alphaStack)) {{1}};
}

void Frame::pushTransform(const sf::Transform transform) {
    sf::Transform nextTransform{transformStack.top()};
    nextTransform.combine(transform);
    transformStack.push(nextTransform);
}

void Frame::popTransform() {
    assert(transformStack.size() > 1);
    transformStack.pop();
}

void Frame::withTransform(const sf::Transform transform, std::function<void()> fn) {
    pushTransform(transform);
    fn();
    popTransform();
}

void Frame::pushAlpha(const float alpha) {
    alphaStack.push(alphaStack.top() * alpha);
}

void Frame::popAlpha() {
    assert(alphaStack.size() > 1);
    alphaStack.pop();
}

void Frame::withAlpha(const float alpha, std::function<void()> fn) {
    pushAlpha(alpha);
    fn();
    popAlpha();
}

void Frame::withAlphaTransform(const float alpha, const sf::Transform transform, std::function<void()> fn) {
    pushAlpha(alpha), pushTransform(transform);
    fn();
    popTransform(), popAlpha();
}

void Frame::drawCircle(const float radius, const sf::Color color) {
    sf::CircleShape circle(radius);
    circle.setFillColor(alphaScaleColor(color));
    window.draw(circle, transformStack.top());
}

/**********************************************************************************************************************/
/* DemoControl
/**********************************************************************************************************************/

void DemoControl::tick(double delta) {
    time += delta;
}

void DemoControl::render(Frame &frame) {
    frame.drawCircle((float) (100 + 50 * std::sin(time)), sf::Color(255, 255, 255));
}

void DemoControl::handle(sf::Event event) { }

/**********************************************************************************************************************/
/* run_sfml
/**********************************************************************************************************************/

void run_sfml(Control &face) {
    app_log(LogLevel::Notice, "Starting app...");

    sf::Clock clock;
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
    window.setVerticalSyncEnabled(true);

    Frame frame(window);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                app_log(LogLevel::Notice, "Caught close signal.");
                window.close();
            }

            face.handle(event);

            float delta = clock.restart().asSeconds();
            face.tick(delta);

            if (event.type == sf::Event::Resized)
                frame.resize();

            frame.reset();
            window.clear(sf::Color::Black);
            face.render(frame);
            window.display();
        }
    }

    app_log(LogLevel::Notice, "Exiting app.");
}

