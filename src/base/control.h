/**
 *
 */
#ifndef SOLEMNSKY_BASE_CTRL_H
#define SOLEMNSKY_BASE_CTRL_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <stack>
#include <functional>
#include <memory>

class Control;

/**
 * Little wrapper around the SFML drawing system, allowing for a simpler design
 * of the methods that use it.
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

  sf::RenderWindow &window; // sometimes we might need this

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
  void drawCircle(const sf::Vector2f pos, const float radius,
                  const sf::Color color = sf::Color());
  void drawRect(const sf::Vector2f topLeft, const sf::Vector2f bottomRight,
                const sf::Color color = sf::Color());
};

/**
 * A user interface with everything you'd expect from one.
 */
class Control {
public:
  /**
   * Sending signals to the app loop.
   */
  std::shared_ptr<Control> next{nullptr};
  bool quitting{false};

  /**
   * Callbacks to integrate into the app loop.
   */
  virtual void tick(float delta) = 0;
  virtual void render(Frame &f) = 0;
  virtual void handle(sf::Event event) = 0;
};

/**
 * Run a Control with SFML, praised be.
 */
void runSFML(std::shared_ptr<Control> ctrl);

#endif //SOLEMNSKY_BASE_CTRL_H
