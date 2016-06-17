/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cmath>
#include "splash.hpp"
#include "util/printer.hpp"

namespace ui {

/**
 * Profiler.
 */

Profiler::Profiler(const unsigned int size) :
    cycleTime(size), logicTime(size),
    renderTime(size), primCount(size) {}

ProfilerSnapshot::ProfilerSnapshot(const Profiler &profiler) :
    cycleTime(profiler.cycleTime), logicTime(profiler.logicTime),
    renderTime(profiler.renderTime) {}

/**
 * AppState.
 */

AppRefs::AppRefs(const AppResources &resources,
                 const Time &time,
                 const sf::RenderWindow &window,
                 const Profiler &profiler) :
    resources(resources),
    uptime(time),
    window(window),
    profiler(profiler) {}

double AppRefs::timeSince(const Time event) const {
  return uptime - event;
}

/**
 * Control.
 */

void Control::areChildren(std::initializer_list<Control *> controls) {
  for (auto control : controls) children.push_back(control);
}

Control::Control(const AppRefs &references) :
    references(references),
    resources(references.resources),
    quitting(false) {}

bool Control::poll() {
  for (auto child : children) {
    while (!child->poll()) {};
  }
  return true;
}

void Control::tick(const TimeDiff delta) {
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
  const TimeDiff cycleDelta = cycleClock.restart().asSeconds();
  profiler.cycleTime.push(cycleDelta);
  uptime += Time(cycleDelta);

  profileClock.restart();
  rollingTickTime += cycleDelta;
  while (!ctrl->poll()) {}
  while (rollingTickTime > tickStep) {
    ctrl->tick(tickStep);
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

ControlExec::ControlExec() :
    window(sf::VideoMode(800, 600), "solemnsky",
           sf::Style::Default, makeSettings()),
    frame(window),
    resizeCooldown(0.5),

    uptime(0),
    tickStep(1.0f / 60.0f),
    rollingTickTime(0),

    profiler(100),

    appState(*((AppResources *) nullptr), uptime, window, profiler) {
  window.setVerticalSyncEnabled(true);
  window.setKeyRepeatEnabled(false);
  appLog("Initialized SFML!", LogOrigin::App);
}

void
ControlExec::run(std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp) {
  ctrl = std::make_unique<detail::SplashScreen>(appState, mkApp);
  appLog("Starting application loop...", LogOrigin::App);

  while (window.isOpen()) {
    tick();
    handle();
    renderAndSleep();

    if (ctrl->quitting) window.close();
  }

  appLog("Exiting cleanly.", LogOrigin::App);
}

void runSFML(std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp) {
  ControlExec().run(mkApp);
}

}

