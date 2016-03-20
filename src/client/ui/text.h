/**
 * Text rendering API used in ui::Frame with wrapping / aligning / spacing.
 */
#pragma once
#include <SFML/Graphics.hpp>

namespace ui {

enum class TextAlign {
  Low, // top / left
  Center,
  High // bottom / right
};

/**
 * Constant formatting parameters in a certain text rendering.
 */
struct TextFormat {
  TextFormat() = delete;
  TextFormat(const int size,
             const sf::Color &bgColor,
             const sf::Color &fgColor,
             const sf::Vector2f &maxDimensions,
             const TextAlign alignX, const TextAlign alignY,
             const sf::Font &font);

  int size;
  sf::Color bgColor, fgColor;
  sf::Vector2f maxDimensions;
  TextAlign alignX, alignY;
  sf::Font const *font;
  float lineWidth, lineSpacing;
};

/**
 * A frame in which a user can draw text; allocated internally by the Frame.
 */
class TextFrame {
 private:
  friend class Frame;
  TextFrame(Frame *parent,
            const sf::Vector2f &anchor,
            const TextFormat &format);
  ~TextFrame();

  // parameters and references
  class Frame *parent;
  const TextFormat &format;
  const sf::Vector2f &anchor;

  // state
  float yOffset;
  float lines = 1;
  sf::Color color;

 public:
  TextFrame() = delete;

  // API
  void drawString(const std::string &string);
  void breakLine();
  void setColor(const sf::Color &color);
};

}
