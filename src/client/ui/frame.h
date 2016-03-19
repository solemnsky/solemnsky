/**
 * A screen we can draw to.
 */
#pragma once
#include <memory>
#include <stack>
#include <SFML/Graphics.hpp>
#include <functional>
#include "client/util/resources.h"
#include "client/ui/textprop.h"

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

  inline float drawText(const sf::Vector2f pos, const std::string &string,
                        const TextProperties &prop = TextProperties::normal) {
    return drawText(pos, {string}, prop);
  }
  inline float drawText(const sf::Vector2f pos, const std::string &string,
                        const int size,
                        const sf::Color &col = sf::Color::White) {
    auto p = TextProperties::normal;
    p.size = size;
    p.color = col;
    return drawText(pos, {string}, p);
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
                 const TextProperties &prop = TextProperties::normal);
  float drawText(const sf::Vector2f &pos,
                 std::initializer_list<std::string> strings,
                 const TextProperties &prop = TextProperties::normal);

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
                      const TextProperties &prop) {
  float yOffset = 0;

  for (Iterator i = beginString; i != endString; i++) {
    std::string string = *i;

    primCount++;
    sf::Text text;
    text.setFont(*prop.font);
    text.setCharacterSize((unsigned int) prop.size);
    text.setStyle(prop.style);
    text.setString(string);

    int lines = 1;

    if (prop.maxWidth > 0) {
      int width = (int) text.getLocalBounds().width;
      if (width > prop.maxWidth) {
        size_t prev = std::string::npos;
        int check = (width % prop.maxWidth) +
            (width / prop.maxWidth) * prop.maxWidth;
        size_t p;
        while ((p = string.find_last_of(' ', prev)) != std::string::npos) {
          int charPos = (int) text.findCharacterPos(p).x;
          if (charPos - check < 0) {
            string.insert(p, "\n");
            check -= prop.maxWidth;
            lines++;
            if (check < prop.maxWidth) break;
          }
          prev = p - 1;
        }
        text.setString(string);
      }
    }

    text.setPosition(pos + sf::Vector2f(0, yOffset));
    text.setColor(alphaScaleColor(prop.color));
    window.draw(text, transformStack.top());

    yOffset += textSize(string, prop.size, *prop.font).y;
  }

  return yOffset;
}

}
