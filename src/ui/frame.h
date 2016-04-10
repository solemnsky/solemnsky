/**
 * A screen we can draw to.
 */
#pragma once
#include <memory>
#include <stack>
#include <SFML/Graphics.hpp>
#include <functional>
#include "client/util/resources.h"
#include "text.h"

namespace ui {
class Control;

class Frame {
  friend class TextFrame;
  friend void runSFML(std::function<std::unique_ptr<Control>()> initCtrl);

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
  void pushTransform(const sf::Transform &transform);
  void popTransform();
  void withTransform(const sf::Transform &transform, std::function<void()> fn);
  void pushAlpha(const float alpha);
  void popAlpha();
  void withAlpha(const float alpha, std::function<void()> fn);
  void withAlphaTransform(const float alpha, const sf::Transform &transform,
                          std::function<void()> fn);

  /**
   * Drawing API.
   */

  inline void drawRect(const sf::FloatRect &rect, const sf::Color &color = {}) {
    drawRect(sf::Vector2f(rect.left, rect.top),
             sf::Vector2f(rect.left + rect.width, rect.top + rect.height),
             color);
  }

  inline sf::Vector2f drawText(
      const sf::Vector2f pos,
      const std::string &string,
      const sf::Color &color, const TextFormat &format) {
    return drawText(pos, [&string](TextFrame &tf) {
      tf.setColor(color);
      tf.print(string);
    }, format);
  }

  void drawCircle(const sf::Vector2f &pos, const float radius,
                  const sf::Color &color = {});
  void drawRect(const sf::Vector2f &topLeft, const sf::Vector2f &bottomRight,
                const sf::Color &color = {});
  sf::Vector2f drawText(const sf::Vector2f &pos,
                        PrintProcess process,
                        const TextFormat &format);
  void drawSprite(const sf::Texture &texture, const sf::Vector2f &pos,
                  const sf::IntRect &portion);
};

}
