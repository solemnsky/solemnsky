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
#include "client/util/resources.h"

namespace ui {
class Control;

class Frame {
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
   * Drawing stuff.
   */

  // some inline synonyms
  inline void drawRect(const sf::FloatRect &rect, const sf::Color &color = {}) {
    drawRect(sf::Vector2f(rect.left, rect.top),
             sf::Vector2f(rect.left + rect.width, rect.top + rect.height),
             color);
  }

  inline void drawRect(const sf::Vector2f &dims, const sf::Color &color = {}) {
    drawRect(-0.5f * dims, 0.5f * dims, color);
  }

  inline void drawSprite(const sf::Texture &texture, const sf::FloatRect &area,
                         const sf::IntRect &portion) {
    withTransform(
        sf::Transform()
            .scale(portion.width / area.width,
                   portion.height / area.height)
            .translate(area.left, area.top),
        [&]() {
          drawSprite(texture, {0, 0}, portion);
        });
  }

  // non-inline methods
  void drawCircle(const sf::Vector2f &pos, const float radius,
                  const sf::Color &color = {});

  void drawRect(const sf::Vector2f &topLeft, const sf::Vector2f &bottomRight,
                const sf::Color &color = {});

  template<typename Iterator>
  void drawText(const sf::Vector2f &pos,
                const Iterator beginString,
                const Iterator endString,
                const int size = 24,
                const sf::Color &color = sf::Color::White,
                const sf::Font &font = fontOf(Res::Font),
                const sf::Text::Style &style = sf::Text::Regular);
  void drawText(const sf::Vector2f &pos,
                std::initializer_list<std::string> strings,
                const int size = 24,
                const sf::Color &color = sf::Color::White,
                const sf::Font &font = fontOf(Res::Font),
                const sf::Text::Style &style = sf::Text::Regular);

  void drawSprite(const sf::Texture &texture, const sf::Vector2f &pos,
                  const sf::IntRect &portion);

  // text sizes
  sf::Vector2f textSize(const std::string contents, const int size = 24,
                        const sf::Font &font = fontOf(Res::Font));
};

template<typename Iterator>
void Frame::drawText(const sf::Vector2f &pos,
                     const Iterator beginString,
                     const Iterator endString,
                     const int size,
                     const sf::Color &color,
                     const sf::Font &font,
                     const sf::Text::Style &style) {
  float yOffset = 0;

  for (Iterator i = beginString; i != endString; i++) {
    const std::string &string = *i;

    primCount++;
    sf::Text text;
    text.setFont(font);
    text.setPosition(pos + sf::Vector2f(0, yOffset));
    text.setString(string);
    text.setCharacterSize((unsigned int) size);
    text.setColor(alphaScaleColor(color));
    text.setStyle(style);
    window.draw(text, transformStack.top());

    yOffset += textSize(string, size, font).y;
  }
}

}
#endif //SOLEMNSKY_FRAME_H
