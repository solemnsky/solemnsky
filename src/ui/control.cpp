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
#include "util/clientutil.hpp"

namespace ui {

/**
 * Profiler.
 */

Profiler::Profiler(const unsigned int size) :
    cycleTime(size), logicTime(size),
    renderTime(size), primCount(size) { }

ProfilerSnapshot::ProfilerSnapshot(const Profiler &profiler) :
    cycleTime(profiler.cycleTime), logicTime(profiler.logicTime),
    renderTime(profiler.renderTime) { }

/**
 * AppState.
 */

AppRefs::AppRefs(Settings &settings,
                 const AppResources &resources,
                 const Time &time,
                 const sf::RenderWindow &window,
                 const Profiler &profiler) :
    settings(settings),
    resources(resources),
    uptime(time),
    window(window),
    profiler(profiler) { }

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
    settings(references.settings),
    quitting(false) { }

void Control::poll() {
  for (auto child : children) child->poll();
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
 * ControlExec.
 */

void ControlExec::tick() {
  const TimeDiff cycleDelta = cycleClock.restart().asSeconds();
  profiler.cycleTime.push(cycleDelta);
  uptime += Time(cycleDelta);

  profileClock.restart();
  rollingTickTime += cycleDelta;
  ctrl->poll();

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
    switch (event.type) {
      case sf::Event::Closed:
        appLog("Caught close signal.", LogOrigin::App);
        window.close();
        break;
      case sf::Event::Resized:
        resizeCooldown.reset();
        frame.resize();
        break;
      case sf::Event::LostFocus:
        ctrl->reset();
        break;

      case sf::Event::MouseWheelScrolled:
        // fk mouse wheels
        break;

      default:
        ctrl->handle(transformEvent(frame.windowToFrame, event));
        ctrl->signalRead();
        ctrl->signalClear();
        break;
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

sf::ContextSettings makeContextSettings(const Settings &settings) {
  sf::ContextSettings csettings;
  csettings.antialiasingLevel = 8;
  return csettings;
}

sf::Uint32 makeDisplayStyle(const Settings &settings) {
  if (settings.fullscreen) {
    return sf::Style::Fullscreen;
  } else {
    return sf::Style::Default;
  }
}

sf::VideoMode makeVideoMode(const Settings &settings) {
  return sf::VideoMode(settings.resolution.x, settings.resolution.y);
}

ControlExec::ControlExec() :
    settingsFile("solemnsky-settings.json"),
    settings(settingsFile),

    window(makeVideoMode(settings), "solemnsky",
           makeDisplayStyle(settings), makeContextSettings(settings)),

    frame(window),
    resizeCooldown(0.5),

    uptime(0),
    tickStep(1.0f / 60.0f),
    rollingTickTime(0),

    profiler(100),

    // Let me apologize in advance for this undefined behaviour (null references
    // are not permitted by The Standard). It was necessary. Only you and I shall
    // ever know of it; it stays a secret between us, yes?

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wnull-dereference"
    appState(settings, *((AppResources *) nullptr), uptime, window, profiler) {
    #pragma clang diagnostic pop
  window.setVerticalSyncEnabled(true);
  window.setKeyRepeatEnabled(false);
  appLog("Initialized SFML!", LogOrigin::App);
}

ControlExec::~ControlExec() {
  ctrl = nullptr;
  settings.writeToFile(settingsFile);
}

void ControlExec::run(std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp) {
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

