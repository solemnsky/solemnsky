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

/**
 * Properties determining how text is drawn.
 */
struct TextProperties {
  TextProperties() = default;

  int size = 24;
  sf::Color color = sf::Color::White;
  int maxWidth = 0;
  bool alignBottom = false;
  sf::Font const *font = &fontOf(Res::Font);
  sf::Text::Style style = sf::Text::Regular;
};

/**
 * Class corresponding to the rendering of a portion of text.
 */
class TextRender {
 private:
  friend class Frame;
  TextRender(const sf::Vector2f &pos, const TextProperties &properties);

  // parameters
  class Frame *parent;
  const sf::Vector2f &pos;
  const TextProperties &properties;

  // state
  float yOffset;
  int lines = 1;
 public:
  void draw(const std::string &str);
  void setColor(const sf::Color &);
};

class Frame {
  friend class TextRender;
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
                        const TextProperties &prop = {}) {
    return drawText(pos, [&string](TextRender &render) {
      render.draw(string);
    }, prop);
  }

  // non-inline methods
  void drawCircle(const sf::Vector2f &pos, const float radius,
                  const sf::Color &color = {});

  void drawRect(const sf::Vector2f &topLeft, const sf::Vector2f &bottomRight,
                const sf::Color &color = {});

  float drawText(const sf::Vector2f &pos,
                 std::function<void(TextRender &)> f,
                 const TextProperties &prop = {});

  void drawSprite(const sf::Texture &texture, const sf::Vector2f &pos,
                  const sf::IntRect &portion);

  // text sizes
  sf::Vector2f textSize(const std::string contents, const int size = 24,
                        const sf::Font &font = fontOf(Res::Font));

};

}
