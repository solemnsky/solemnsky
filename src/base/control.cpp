#include <cmath>
#include "profiler.h"
#include "util.h"
#include "control.h"

namespace detail {
/****
 * transformEvent
 * Fakes a transformation of any potential position-related values on an
 * event. The fact that these position-related values are all ints makes me
 * feel like we should make our own event type, just out of respect for
 * semantics. However, this doesn't realistically do us anything as long as
 * the window dimensions are less than 1600x900. I suppose I expect to come
 * back to this sometime in the future when everybody's using 4k displays...
 */

sf::Event detail::transformEvent(const sf::Transform trans,
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

    // this would be so much cooler in Haskell, trust me

    return newEvent;
  }

  return event;
}
}

/****
 * Frame
 */

const sf::Color Frame::alphaScaleColor(const sf::Color color) {
  sf::Color newColor(color);
  newColor.a *= alphaStack.top();
  return newColor;
}

Frame::Frame(sf::RenderWindow &window) : window(window) {
  resize();
}

void Frame::resize() {
  sf::Vector2u size = window.getSize();
  float sx(size.x), sy(size.y);
  const float viewAspect = sx / sy;
  static constexpr float targetAspect = 16.0f / 9.0f;

  // set the view
  sf::View view;
  view.setCenter(800, 450);
  if (viewAspect > targetAspect) {
    view.setSize(1600 * (viewAspect / targetAspect), 900);
    float scalar = 900 / sy;
    windowToFrame = sf::Transform().scale(scalar, scalar);
    windowToFrame.translate(-(sx - (sy * targetAspect)) / 2, 0);
  }
  else {
    view.setSize(1600, 900 * (targetAspect / viewAspect));
    float scalar = 1600 / sx;
    windowToFrame = sf::Transform().scale(scalar, scalar);
    windowToFrame.translate(0, -(sy - (sx / targetAspect)) / 2);
  }

  window.setView(view);
}

void Frame::beginDraw() {
  primCount = 0;
  transformStack = (decltype(transformStack)) {{sf::Transform()}};
  alphaStack = (decltype(alphaStack)) {{1}};
  window.clear(sf::Color::Black);
}

void Frame::endDraw() {
  sf::Vector2f topLeft = windowToFrame.transformPoint(sf::Vector2f(0, 0));
  sf::Vector2f bottomRight = windowToFrame.transformPoint(
      sf::Vector2f(1600, 900));

  sf::Color color = sf::Color(20, 20, 20, 255);
  drawRect(topLeft, sf::Vector2f(0, 900), color);
  drawRect(sf::Vector2f(1600, 0), bottomRight, color);
  drawRect(topLeft, sf::Vector2f(1600, 0), color);
  drawRect(sf::Vector2f(0, 900), bottomRight, color);
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

void Frame::withTransform(const sf::Transform transform,
                          std::function<void()> fn) {
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

void Frame::withAlphaTransform(const float alpha, const sf::Transform transform,
                               std::function<void()> fn) {
  pushAlpha(alpha), pushTransform(transform);
  fn();
  popTransform(), popAlpha();
}

void Frame::drawCircle(const sf::Vector2f pos, const float radius,
                       const sf::Color color) {
  primCount++;
  sf::CircleShape circle(radius);
  circle.setPosition(pos - sf::Vector2f(radius, radius));
  circle.setFillColor(alphaScaleColor(color));
  window.draw(circle, transformStack.top());
}

void Frame::drawRect(const sf::Vector2f topLeft, const sf::Vector2f bottomRight,
                     const sf::Color color) {
  primCount++;
  sf::RectangleShape rect(bottomRight - topLeft);
  rect.setPosition(topLeft);
  rect.setFillColor(alphaScaleColor(color));
  window.draw(rect, transformStack.top());
}


/****
 * runSFML
 */

void runSFML(std::shared_ptr<Control> ctrl) {
  Optional<std::shared_ptr<Control>> passedOn{};

  appLog(LogType::Notice, "Creating window ...");

  sf::ContextSettings settings;
  settings.antialiasingLevel = 8;
  sf::RenderWindow window(sf::VideoMode(800, 600), "My window",
                          sf::Style::Default, settings);
  window.setVerticalSyncEnabled(true);

  appLog(LogType::Notice, "Starting application ...");

  Profiler profiler{200};
  Ticker profileTicker{200}, resizeTicker{1};
  profileTicker.wait(); // wait 200 cycles before emitting a profile

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
     * Rendering / Sleeping
     * Here our goal is to update the window display (at the next available screen refresh), and in doing so
     * spend some time before ticking.
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
     * Here our goal is to execute a logic routine as many times as it needs to simulate the game that passed
     * during the time we slept.
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

    /*
     * Events
     * Here we process any events that were added to the cue during our rendering and logic.
     */
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        appLog(LogType::Notice, "Caught close signal.");
        window.close();
      }

      if (event.type != sf::Event::MouseWheelScrolled) // fk mouse wheels
        ctrl->handle(detail::transformEvent(frame.windowToFrame, event));
    }

    if (profileTicker.tick(1)) appLog(LogType::Info, profiler.print());
  }

  appLog(LogType::Notice, "Exiting app.");
}

