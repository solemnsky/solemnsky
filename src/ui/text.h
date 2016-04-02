/**
 * Text rendering API with aligning / wrapping.
 */
#pragma once
#include <SFML/Graphics.hpp>
#include "client/util/resources.h"

namespace ui {

/**
 * TODO
 * This will necessarily be updated in the future when we need:
 * 1) Word wrapping.
 * 2) Horizontal centering of multi-colored text.
 * 3) Vertical centering of multi-lined text.
 *
 * The API should remain virtually unchanged though, so this is a
 * low-priority issue.
 */

enum class HorizontalAlign {
  Left, // displace strings to the right as you draw them
  Center, // draw one string, centered horizontally on the anchor
  Right // displace strings to the left as you draw them
};

enum class VerticalAlign {
  Top, // displace lines to the bottom as you draw them
  Middle, // draw one line, centered vertically on the anchor
  Bottom // displace lines to the top as you draw them
};

/**
 * Constant formatting parameters in a certain text rendering.
 */
struct TextFormat {
  TextFormat() = delete;
  TextFormat(const int size,
             const sf::Vector2f &maxDimensions,
             const HorizontalAlign horizontal, const VerticalAlign vertical,
             const ResID &font);

  int size;
  sf::Vector2f maxDimensions; // values of 0 mean there is no limit
  HorizontalAlign horizontal;
  VerticalAlign vertical;
  ResID font; // resource ID
};

/**
 * A frame in which a user can draw text; allocated internally by the Frame.
 */
class TextFrame {
 private:
  friend class Frame;

  TextFrame(class Frame *parent,
            const sf::Vector2f &anchor,
            const sf::Color &color,
            const TextFormat &format);
  TextFrame() = delete;

  // parameters and references
  class Frame *parent;
  const TextFormat &format;
  const sf::Vector2f &anchor;

  const sf::Font &font;

  // state
  sf::Color color;
  sf::Vector2f drawnDimensions;

  sf::Vector2f drawBlock(const sf::Vector2f &pos,
                         const std::string &string);
  sf::Vector2f endRender();

 public:
  sf::Vector2f drawOffset;

  // API
  void drawString(const std::string &string);
  void breakLine();
  void setColor(const sf::Color &newColor);
};

}
