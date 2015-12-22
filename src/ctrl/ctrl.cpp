#include <SFML/Graphics.hpp>
#include "log.h"
#include <iostream>
#include "ctrl.h"
#include "profile.h"
#include "util/util.h"

/**********************************************************************************************************************/
/* Frame
/**********************************************************************************************************************/

const sf::Color Frame::alphaScaleColor(const sf::Color color) {
    sf::Color newColor(color);
    newColor.a *= alphaStack.top();
    return newColor;
}

Frame::Frame(sf::RenderWindow &window) : window{window} {
}

void Frame::resize() {
    auto size = window.getSize();
    const float viewAspect = (float) size.x / (float) size.y;
    static constexpr float targetAspect = 16.0f / 9.0f;

    sf::View view;
    view.setCenter(800, 450);
    if (viewAspect > targetAspect) view.setSize(1600 * (viewAspect / targetAspect), 900);
    else view.setSize(1600, 900 * (targetAspect / viewAspect));

    window.setView(view);
}

void Frame::beginDraw() {
    primCount = 0;
    transformStack = (decltype(transformStack)) {{sf::Transform()}};
    alphaStack = (decltype(alphaStack)) {{1}};
    window.clear(sf::Color::Black);
}

void Frame::endDraw() {
    // TODO: draw frame borders
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

void Frame::drawCircle(const sf::Vector2f pos, const float radius, const sf::Color color) {
    primCount++;
    sf::CircleShape circle(radius);
    circle.setPosition(pos - sf::Vector2f(radius, radius));
    circle.setFillColor(alphaScaleColor(color));
    window.draw(circle, transformStack.top());
}

/**********************************************************************************************************************/
/* runSFML
/**********************************************************************************************************************/

static sf::Event transformEvent(sf::RenderWindow &window, const sf::Event event) {
    if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f pos(event.mouseMove.x, event.mouseMove.y);
        pos = window.getView().getTransform().transformPoint(pos);

        sf::Event::MouseMoveEvent newMouseMove;
        sf::Event newEvent;

        newMouseMove.x = (int) pos.x, newMouseMove.y = (int) pos.y;
        newEvent.type = sf::Event::MouseMoved;
        newEvent.mouseMove = newMouseMove;

        return newEvent;
    }

    return event;
}

void runSFML(Control &face) {
    app_log(LogType::Notice, "Creating window ...");

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    app_log(LogType::Notice, "Starting application ...");

    Profiler profiler{200};
    Ticker profileTicker{200}, resizeTicker{1};
//    profileTicker.wait();

    sf::Clock cycleClock, profileClock;
    const float tickStep = 1 / 60.0f;
    float rollingTickTime = 0;
    float cycleDelta = 0;

    Frame frame(window);
    sf::Event event;

    while (window.isOpen()) {
        /**
         * Rendering / Sleeping
         * Here our goal is to update the window display (at the next available screen refresh), and in doing so
         * spend some time before ticking.
         */
        if (event.type == sf::Event::Resized || resizeTicker.tick(cycleDelta)) frame.resize();
        frame.beginDraw();
        profileClock.restart();
        face.render(frame);
        profiler.renderTime.push(profileClock.restart().asSeconds());
        profiler.primCount.push(frame.primCount);
        frame.endDraw();
        window.display();
        // window.display() doesn't seem to block when the window isn't focused
        if (!window.hasFocus()) sf::sleep(sf::milliseconds(16));

        /**
         * Ticking
         * Here our goal is to execute a logic routine as many times as it needs to simulate the game that passed
         * during the time we slept.
         */
        cycleDelta = cycleClock.restart().asSeconds();
        profiler.cycleTime.push(cycleDelta);

        profileClock.restart();
        rollingTickTime += cycleDelta;
        while (rollingTickTime > tickStep) {
            face.tick(tickStep);
            rollingTickTime -= tickStep;
        }
        profiler.logicTime.push(profileClock.restart().asSeconds());

        /**
         * Events
         * Here we process any events that were added to the cue during our rendering and logic.
         */
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                app_log(LogType::Notice, "Caught close signal.");
                window.close();
            }
            face.handle(transformEvent(window, event));
        }

        if (profileTicker.tick(1)) app_log(LogType::Info, profiler.print());
    }

    app_log(LogType::Notice, "Exiting app.");
}

