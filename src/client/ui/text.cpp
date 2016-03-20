#include "text.h"
#include "frame.h"

namespace ui {

/**
 * TextFormat.
 */
TextFormat::TextFormat(
    const int size,
    const sf::Vector2f &maxDimensions,
    const HorizontalAlign horizontal,
    const VerticalAlign vertical,
    const sf::Font &font) :
    size(size),
    maxDimensions(maxDimensions),
    horizontal(horizontal),
    vertical(vertical),
    font(&font) { }

/**
 * TextFrame.
 */

TextFrame::TextFrame(TextFrame::Frame *parent,
                     const sf::Vector2f &anchor,
                     const sf::Color &color,
                     const TextFormat &format) :
    parent(parent), anchor(anchor), format(format),
    color(color) { }

sf::Vector2f TextFrame::endRender() {
  return drawnDimensions;
}

void TextFrame::drawString(const std::string &string) {
  // TODO: STUB

  parent->primCount++;

  sf::Text text;
  text.setFont(*format.font);
  text.setCharacterSize((unsigned int) format.size);
  text.setString(string);

  const float maxWidth = format.maxDimensions.x;
  std::string brokenString(string);

  if (maxWidth > 0) {
    int width = (int) text.getLocalBounds().width;
    if (width > format.maxDimensions.x) {
      size_t prev = std::string::npos;
      float check = (width / maxWidth) + (width / maxWidth) * maxWidth;
      size_t p;
      while ((p = brokenString.find_last_of(' ', prev)) != std::string::npos) {
        int charPos = (int) text.findCharacterPos(p).x;
        if (charPos - check < 0) {
          brokenString.insert(p, "\n");
          check -= maxWidth;
          lines++;
          if (check < maxWidth) break;
        }
        prev = p - 1;
      }
      text.setString(brokenString);
    }
  }

  // TODO: drawnDims
  text.setPosition(anchor + sf::Vector2f(0, yOffset));
  text.setColor(parent->alphaScaleColor(color));
  parent->window.draw(text, parent->transformStack.top());
  yOffset += text.getLocalBounds().height;
}

void TextFrame::breakLine() {

}

void TextFrame::setColor(const sf::Color &color) {

}

}
