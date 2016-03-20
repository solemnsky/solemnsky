/**
 * Text rendering API used in ui::Frame with wrapping / aligning / spacing.
 */
#pragma once
#include <SFML/Graphics.hpp>
#include "client/util/resources.h"

namespace ui {

enum class HorizontalAlign { Left, Center, Right };
enum class VerticalAlign { Top, Middle, Bottom };

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

  // state
  float yOffset;
  float lines = 1;
  sf::Color color;

  sf::Vector2f endRender();
  sf::Vector2f drawnDimensions;

 public:
  // API
  void drawString(const std::string &string);
  void breakLine();
  void setColor(const sf::Color &color);
};

}
