#include <cmath>

#include "wrapper.h"
#include "util/printer.h"

namespace ui {

/**
 * Profiler.
 */

Profiler::Profiler(int size) :
    cycleTime(size), logicTime(size),
    renderTime(size), primCount(size) { }

ProfilerSnapshot::ProfilerSnapshot(const Profiler &profiler) :
    cycleTime(profiler.cycleTime), logicTime(profiler.logicTime),
    renderTime(profiler.renderTime) { }

/**
 * AppState.
 */

AppState::AppState(const sf::RenderWindow &window,
                   const Profiler &profiler,
                   const double &time) :
    time(time), window(window), profiler(profiler) { }

double AppState::timeSince(const double event) const {
  return time - event;
}

/**
 * Control.
 */

Control::Control(AppState &appState) :
    appState(appState),
    quitting(false) { }

void Control::poll(float delta) {
  for (auto child : children) child->poll(delta);
}

void Control::tick(float delta) {
  for (auto child : children) child->tick(delta);
}

void Control::render(Frame &f) {
  for (auto child : children) child->render(f);
}

bool Control::handle(const sf::Event &event) {
  for (auto child : children) if (child->handle(event)) return true;
  return false;
}

void Control::reset() {
  for (auto child : children) child->reset();
}

void Control::signalRead() { // process signals
  for (auto child : children) child->signalRead();
}

void Control::signalClear() { // clear signals
  for (auto child : children) child->signalClear();
}

/**
 * Fakes a transformation of any potential position-related values on an
 * event. Unfortunately this doesn't preserve precision, but the alternative
 * would be to wrap sf::Event... it's not worth it, at least until people
 * complain about issues on their 4k systems.
 */
sf::Event transformEvent(const sf::Transform trans,
                         const sf::Event event) {
  const bool mouseMoved(event.type == sf::Event::MouseMoved),
      mousePressed(event.type == sf::Event::MouseButtonPressed ||
      event.type == sf::Event::MouseButtonReleased),
      mouseSomething(mouseMoved || mousePressed);

  if (mouseSomething) {
    sf::Vector2f pos;
    if (mouseMoved) {
      pos = trans.transformPoint(
          sf::Vector2f(event.mouseMove.x, event.mouseMove.y));
    }
    if (mousePressed) {
      pos = trans.transformPoint(
          sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
    }

    sf::Event newEvent;
    newEvent.type = event.type;

    if (mouseMoved) {
      sf::Event::MouseMoveEvent newMouseMove;
      newMouseMove.x = (int) std::round(pos.x);
      newMouseMove.y = (int) std::round(pos.y);
      newEvent.mouseMove = newMouseMove;
    }
    if (mousePressed) {
      sf::Event::MouseButtonEvent newMouseButton;
      newMouseButton.x = (int) std::round(pos.x);
      newMouseButton.y = (int) std::round(pos.y);
      newMouseButton.button = event.mouseButton.button;
      newEvent.mouseButton = newMouseButton;
    }

    return newEvent;
  }

  return event;
}

/**
 * ControlExec.
 */

void ControlExec::tick() {
  const float cycleDelta = cycleClock.restart().asSeconds();
  profiler.cycleTime.push(cycleDelta);

  profileClock.restart();
  rollingTickTime += cycleDelta;
  ctrl->poll(cycleDelta);
  while (rollingTickTime > tickStep) {
    ctrl->tick(tickStep);
    time += tickStep;
    rollingTickTime -= tickStep;
  }

  profiler.logicTime.push(profileClock.restart().asSeconds());

  ctrl->signalRead();
  ctrl->signalClear();

  if (resizeCooldown.cool(cycleDelta)) {
    resizeCooldown.reset();
    frame.resize();
  }
}

void ControlExec::handle() {
  static sf::Event event;
  while (window.pollEvent(event)) {
    if (event.type == sf::Event::Closed) {
      appLog("Caught close signal.", LogOrigin::App);
      window.close();
    }

    if (event.type == sf::Event::Resized) {
      resizeCooldown.reset();
      frame.resize();
    }

    if (event.type == sf::Event::LostFocus) ctrl->reset();

    if (event.type != sf::Event::MouseWheelScrolled) { // fk mouse wheels
      ctrl->handle(transformEvent(frame.windowToFrame, event));
      ctrl->signalRead();
      ctrl->signalClear();
    }
  }
}

void ControlExec::renderAndSleep() {
  frame.beginDraw();
  profileClock.restart();
  ctrl->render(frame);
  profiler.renderTime.push(profileClock.restart().asSeconds());
  profiler.primCount.push(frame.primCount);
  frame.endDraw();
  window.display();
  // window.display() doesn't seem to block when the window isn't focused
  // on certain platforms
  if (!window.hasFocus()) sf::sleep(sf::milliseconds(16));
}

sf::ContextSettings ControlExec::makeSettings() {
  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  return settings;
}

ControlExec::ControlExec(
    std::function<std::unique_ptr<Control>(AppState &)> initCtrl) :
    window(sf::VideoMode(800, 600), "solemnsky",
           sf::Style::Default, makeSettings()),
    frame(window),
    resizeCooldown(0.5),
    time(0),

    profiler(100),
    tickStep(1.0f / 60.0f),
    rollingTickTime(0),

    appState(window, profiler, time),
    ctrl(std::make_unique<detail::ExecWrapper>(appState, initCtrl)) {
  window.setVerticalSyncEnabled(true);
  window.setKeyRepeatEnabled(false);
  appLog("Initialized SFML.", LogOrigin::App);
}

void ControlExec::run() {
  appLog("Starting game loop.", LogOrigin::App);

  while (window.isOpen()) {
    tick();
    handle();
    renderAndSleep();

    if (ctrl->quitting) window.close();
  }

  appLog("Exiting solemnsky, see you later.", LogOrigin::App);
}

void runSFML(std::function<std::unique_ptr<Control>(AppState &)> initCtrl) {
  ControlExec(initCtrl).run();
}

}

