#include <cmath>

#include "splash.h"
#include "util/methods.h"
#include "telegraph/telegraph.h"
#include "control.h"

namespace ui {

/**
 * Profiler.
 */

Profiler::Profiler(int size) :
    cycleTime{size}, logicTime{size}, renderTime{size}, primCount{size},
    size{size} {
  cycleTime.push(0), cycleTime.push(0), renderTime.push(0), primCount.push(0);
  // for safety
}

/**
 * runSFML.
 */

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

void runSFML(std::function<std::unique_ptr<Control>()> initCtrl) {
  std::unique_ptr<Control> ctrl =
      std::make_unique<detail::SplashScreen>(initCtrl);

  /**
   * Create the window.
   */
  appLog("Creating window / various things ...", LogOrigin::App);

  tg::UsageFlag flag; // for ENet initialization

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window",
                          sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true);
  window.setKeyRepeatEnabled(false);

  ui::Profiler profiler{100};
  Cooldown resizeCooldown{1};

  sf::Clock cycleClock, profileClock;
  constexpr float tickStep = 1 / 60.0f;
  float rollingTickTime = 0, cycleDelta = 0;

  Frame frame(window);
  sf::Event event;
  appLog("Finished creating window.", LogOrigin::App);

  /**
   * Create appState, start game loop.
   */
  appLog("Running application...", LogOrigin::App);

  AppState appState(&window, &profiler);

  ctrl->appState = &appState;
  ctrl->attachState();

  while (window.isOpen()) {
    if (ctrl->quitting) window.close();
    if (ctrl->next) {
      ctrl = std::move(ctrl->next);
      ctrl->appState = &appState;
      ctrl->attachState();
    }

    /*
     * Ticking
     * Here our goal is to execute a logic routine as many times as it needs
     * to simulate the game that passed during the time we slept.
     */
    cycleDelta = cycleClock.restart().asSeconds();
    profiler.cycleTime.push(cycleDelta);

    profileClock.restart();
    rollingTickTime += cycleDelta;
    while (rollingTickTime > tickStep) {
      ctrl->tick(tickStep);
      rollingTickTime -= tickStep;
    }
    profiler.logicTime.push(profileClock.restart().asSeconds());

    ctrl->signalRead();
    ctrl->signalClear();

    /*
     * Events
     * Here we process any events that were added to the cue during our
     * rendering and logic, and push our 'signals' through the stack.
     */
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        appLog("Caught close signal.", LogOrigin::App);
        window.close();
      }

      if (event.type != sf::Event::MouseWheelScrolled) { // fk mouse wheels
        ctrl->handle(transformEvent(frame.windowToFrame, event));
        ctrl->signalRead();
        ctrl->signalClear();
      }
    }

    /*
     * Rendering / Sleeping
     * Here our goal is to update the window display (at the next available
     * screen refresh), and in doing so spend some time before ticking.
     */
    if (event.type == sf::Event::Resized || resizeCooldown.cool(cycleDelta)) {
      resizeCooldown.reset();
      frame.resize();
    }
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

  appLog("Exiting app, goodbye!", LogOrigin::App);
}

}

