/**
 * Represents a screen / framebuffer to which you can draw stuff on a
 * per-update basis. Supports handy things like transform / alpha stacks.
 */
#ifndef SOLEMNSKY_FRAME_H
#define SOLEMNSKY_FRAME_H

#include <memory>
#include <stack>
#include <SFML/Graphics.hpp>
#include <functional>

namespace ui {
class Control;

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

  // some inline synonyms
  inline void drawRect(const sf::FloatRect rect, sf::Color color = {}) {
    drawRect(sf::Vector2f(rect.top, rect.left),
             sf::Vector2f(rect.top + rect.height, rect.left + rect.width),
             color);
  }


  // non-inline methods
  void drawCircle(const sf::Vector2f pos, const float radius,
                  const sf::Color color = {});
  void drawRect(const sf::Vector2f topLeft, const sf::Vector2f bottomRight,
                const sf::Color color = {});
};
}
#endif //SOLEMNSKY_FRAME_H
