/**
 * A screen we can draw to.
 */
#pragma once
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

  inline float drawText(const sf::Vector2f pos,
                       const std::string &string,
                       const int size = 24,
                       const sf::Color &color = sf::Color::White,
                       const int maxWidth = 0,
                       const bool alignBottom = false,
                       const sf::Font &font = fontOf(Res::Font),
                       const sf::Text::Style &style = sf::Text::Regular) {
    return drawText(pos, {string}, size, color, maxWidth, alignBottom, font, style);
  }

  // non-inline methods
  void drawCircle(const sf::Vector2f &pos, const float radius,
                  const sf::Color &color = {});

  void drawRect(const sf::Vector2f &topLeft, const sf::Vector2f &bottomRight,
                const sf::Color &color = {});

  template<typename Iterator>
  float drawText(const sf::Vector2f &pos,
                const Iterator beginString,
                const Iterator endString,
                const int size = 24,
                const sf::Color &color = sf::Color::White,
                const int maxWidth = 0,
                const bool alignBottom = false,
                const sf::Font &font = fontOf(Res::Font),
                const sf::Text::Style &style = sf::Text::Regular);
  float drawText(const sf::Vector2f &pos,
                std::initializer_list<std::string> strings,
                const int size = 24,
                const sf::Color &color = sf::Color::White,
                const int maxWidth = 0,
                const bool alignBottom = false,
                const sf::Font &font = fontOf(Res::Font),
                const sf::Text::Style &style = sf::Text::Regular);

  void drawSprite(const sf::Texture &texture, const sf::Vector2f &pos,
                  const sf::IntRect &portion);

  // text sizes
  sf::Vector2f textSize(const std::string contents, const int size = 24,
                        const sf::Font &font = fontOf(Res::Font));

};

template<typename Iterator>
float Frame::drawText(const sf::Vector2f &pos,
                     const Iterator beginString,
                     const Iterator endString,
                     const int size,
                     const sf::Color &color,
                     int maxWidth,
                     const bool alignBottom,
                     const sf::Font &font,
                     const sf::Text::Style &style){
  float yOffset = 0;

  for (Iterator i = beginString; i != endString; i++) {
    std::string string = *i;

    primCount++;
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize((unsigned int) size);
    text.setStyle(style);
    text.setString(string);

    int lines = 1;

    if (maxWidth > 0){
      int width = text.getLocalBounds().width;
      if (width > maxWidth){
        if (maxWidth < 1) maxWidth = 100;
        size_t prev = std::string::npos;
        int check = (width % maxWidth) + (width / maxWidth) * maxWidth;
        size_t p;
        while ((p = string.find_last_of(' ', prev)) != std::string::npos){
          int charPos = text.findCharacterPos(p).x;
          if (charPos - check < 0){
            string.insert(p, "\n");
            check -= maxWidth;
            lines++;
            if (check < maxWidth) break;
          }
          prev = p - 1;
        }
        text.setString(string);
      }
    }

    text.setPosition(pos + sf::Vector2f(0, yOffset));
    text.setColor(alphaScaleColor(color));
    window.draw(text, transformStack.top());

    yOffset += textSize(string, size, font).y;
  }

  return yOffset;
}

}
