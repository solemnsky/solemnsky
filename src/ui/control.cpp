#include <cmath>
#include <assert.h>
#include "base/base.h"
#include "profiler.h"
#include "control.h"

namespace {
/****
 * transformEvent
 * Fakes a transformation of any potential position-related values on an
 * event. The fact that these position-related values are all ints makes me
 * feel like we should make our own event type, just out of respect for
 * semantics. However, this doesn't realistically do us anything as long as
 * the window dimensions are less than 1600x900. I suppose I expect to come
 * back to this sometime in the future when everybody's using 4k displays...
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

    // it almost feels like the SFML API doesn't want us to touch this.. ^^

    return newEvent;
  }

  return event;
}
}

namespace ui {

/****
 * runSFML
 */

void runSFML(std::shared_ptr<Control> ctrl) {
  optional <std::shared_ptr<Control>> passedOn{};

  appLog(LogType::Notice, "Creating window ...");

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window",
                          sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true);

  appLog(LogType::Notice, "Starting application ...");

  Profiler profiler{100};
  Cooldown profileTicker{500}, resizeTicker{1};
  profileTicker.wait();

  sf::Clock cycleClock, profileClock;
  const float tickStep = 1 / 60.0f;
  float rollingTickTime = 0;
  float cycleDelta = 0;

  Frame frame(window);
  sf::Event event;

  while (window.isOpen()) {
    if (ctrl->quitting) window.close();
    if (ctrl->next) ctrl = ctrl->next;

    /*
     * Events
     * Here we process any events that were added to the cue during our
     * rendering and logic.
     */
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        appLog(LogType::Notice, "Caught close signal.");
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
    if (event.type == sf::Event::Resized || resizeTicker.tick(cycleDelta)) {
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
    if (!window.hasFocus()) sf::sleep(sf::milliseconds(16));

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

    if (profileTicker.tick(1)) appLog(LogType::Info, profiler.print());
  }

  appLog(LogType::Notice, "Exiting app.");
}
}

