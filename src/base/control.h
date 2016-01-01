/**
 * Basic structure for our UI abstraction system. Things get messy fast without
 * one...
 */
#ifndef SOLEMNSKY_BASE_CTRL_H
#define SOLEMNSKY_BASE_CTRL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <functional>
#include <memory>

namespace detail {
sf::Event transformEvent(const sf::Transform trans,
                         const sf::Event event);
}

class Control;


/**
 * Represents a screen / framebuffer to which you can draw stuff on a
 * per-update basis. Supports handy things like transform / alpha stacks.
 */
class Frame {
  friend void runSFML(std::shared_ptr<Control> initCtrl);

private:
  std::stack<sf::Transform> transformStack;
  std::stack<float> alphaStack;
  const sf::Color alphaScaleColor(const sf::Color);

  // used by runSFML
  sf::Transform windowToFrame;

  int primCount;
  void beginDraw();
  void endDraw();
  void resize();

public:
  Frame(sf::RenderWindow &window);

  sf::RenderWindow &window; // might be useful for some settings sometimes

  /**
   * Managing the render state stacks. It's like a scene graph but quick.
   */
  void pushTransform(const sf::Transform transform);
  void popTransform();
  void withTransform(const sf::Transform transform, std::function<void()> fn);
  void pushAlpha(const float alpha);
  void popAlpha();
  void withAlpha(const float alpha, std::function<void()> fn);
  void withAlphaTransform(const float alpha, const sf::Transform transform,
                          std::function<void()> fn);

  /**
   * Drawing stuff.
   */

  // some synonyms
  inline void drawRect(const sf::Vector2f dimensions) {
    drawRect(-dimensions, dimensions);
  }

  // the real stuff
  void drawCircle(const sf::Vector2f pos, const float radius,
                  const sf::Color color = sf::Color());
  void drawRect(const sf::Vector2f topLeft, const sf::Vector2f bottomRight,
                const sf::Color color = sf::Color());
};

/**
 * A part of a user interface; evolves over time, can draw to the screen, and
 * can handle user input events.
 */
class Control {
public:
  /**
   * Signals to the app loop.
   */
  std::shared_ptr<Control> next{nullptr};
  bool quitting{false};

  /**
   * Callbacks to the app loop.
   */
  virtual void tick(float delta) = 0;
  virtual void render(Frame &f) = 0;
  virtual void handle(sf::Event event) = 0;
};

/****
 * Wrap a control with some settings (transformation, alpha, etc).
 */

struct WrapSettings {
  sf::Transform transform; // TODO: is caching the inverse of this useful?
  float alpha;
  float timeScale; // larger values make time move more quickly

  WrapSettings() : transform(), alpha(1), timeScale(1) { }

  WrapSettings(sf::Transform transform, float alpha = 1, float timeScale = 1)
      : transform(transform), alpha(alpha), timeScale(timeScale) {
  }
};

template<typename T>
class WrapControl : public Control {
public:
  std::shared_ptr<T> ctrl; // the underlying Control object

  WrapSettings settings;

  WrapControl<T>() = delete;
  WrapControl<T>(std::shared_ptr<T> ctrl, const WrapSettings = {});

  virtual void tick(float delta) override;
  virtual void render(Frame &f) override;
  virtual void handle(sf::Event event) override;
};

template<typename T>
WrapControl<T>::WrapControl(std::shared_ptr<T> ctrl,
                            const WrapSettings settings) :
    ctrl(ctrl), settings(settings) { }

template<typename T>
void WrapControl<T>::tick(float delta) {
  ctrl->tick(settings.timeScale * delta);
}

template<typename T>
void WrapControl<T>::render(Frame &f) {
  f.withAlphaTransform(settings.alpha, settings.transform,
                       [&]() { ctrl->render(f); });
}

template<typename T>
void WrapControl<T>::handle(sf::Event event) {
  ctrl->handle(detail::transformEvent(settings.transform.getInverse(), event));
}

/**
 * Finally, we can turn all this into an application with runSFML, using the
 * SFML 2 lib.
 */
void runSFML(std::shared_ptr<Control> ctrl);

#endif //SOLEMNSKY_BASE_CTRL_H
