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

  sf::Font baseFont;

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
    drawRect(sf::Vector2f(rect.left, rect.top),
             sf::Vector2f(rect.left + rect.width, rect.top + rect.height),
             color);
  }


  // non-inline methods
  void drawCircle(const sf::Vector2f pos, const float radius,
                  const sf::Color color = {});

  void drawRect(const sf::Vector2f topLeft, const sf::Vector2f bottomRight,
                const sf::Color color = {});

  void drawText(const sf::Vector2f pos,
                const std::string contents, const int size = 24,
                const sf::Color color = sf::Color::White,
                const sf::Text::Style style = sf::Text::Regular);

  void drawSprite(const sf::Image image, const sf::Vector2f topLeft,
                  const sf::Vector2f bottomRight,
                  const sf::Vector2f topLeftImg,
                  const sf::Vector2f bottomRightImg);

  // text sizes
  sf::Vector2f textSize(const std::string contents, const int size = 24);
};
}
#endif //SOLEMNSKY_FRAME_H
