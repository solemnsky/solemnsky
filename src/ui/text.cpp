#include "text.h"
#include <cmath>
#include "frame.h"

namespace ui {

float alignValue(const HorizontalAlign align) {
  switch (align) {
    case HorizontalAlign::Left:
      return 0;
    case HorizontalAlign::Center:
      return 0.5;
    case HorizontalAlign::Right:
      return 1;
  }
}

float alignValue(const VerticalAlign align) {
  switch (align) {
    case VerticalAlign::Top:
      return 0;
    case VerticalAlign::Middle:
      return 0.5;
    case VerticalAlign::Bottom:
      return 1;
  }
}

/**
 * TextFormat.
 */
TextFormat::TextFormat(
    const int size,
    const float maxWidth,
    const HorizontalAlign horizontal,
    const VerticalAlign vertical,
    const ResID &font) :
    size(size),
    maxWidth(maxWidth),
    horizontal(horizontal),
    vertical(vertical),
    font(font) { }

/**
 * TextFrame.
 */

TextFrame::TextFrame(Frame *parent,
                     const sf::Vector2f &anchor,
                     const TextFormat &format) :
    parent(parent), anchor(anchor), format(format),
    font(fontOf(format.font)) { }

sf::Vector2f TextFrame::endRender() {
  return drawnDimensions;
}

sf::Vector2f TextFrame::drawBlock(const sf::Vector2f &pos,
                                  const std::string &string) {
  sf::Text text;
  text.setFont(font);
  text.setCharacterSize((unsigned int) format.size);
  text.setString(string);

  const auto bounds = text.getLocalBounds();
  const sf::Vector2f dims = {bounds.width, bounds.height};

  sf::Vector2f drawPos =
      anchor - sf::Vector2f(alignValue(format.horizontal) * dims.x, 0)
          + drawOffset;
  if (format.vertical == VerticalAlign::Middle) {
    drawPos.y -= bounds.top + 0.5f * dims.y;
  } else {
    drawPos.y -= alignValue(format.vertical) * float(format.size);
    drawnDimensions.x =
        std::max(std::abs(drawOffset.x) + dims.x, drawnDimensions.x);
    drawnDimensions.y =
        std::max(std::abs(drawOffset.y) + dims.y, drawnDimensions.y);
  }
  text.setPosition(drawPos);

  text.setColor(parent->alphaScaleColor(color));
  parent->window.draw(text, parent->transformStack.top());
  return {bounds.width, float(format.size)};
}

void TextFrame::print(const std::string &string) {
  assert(colorSet);
  parent->primCount++;

  sf::Text text;
  text.setFont(fontOf(format.font));
  text.setCharacterSize((unsigned int) format.size);
  text.setString(string);

//   TODO: word wrapping
//  const float maxWidth = format.maxDimensions.x;
//  std::string brokenString(string);
//
//  if (maxWidth > 0) {
//    int width = (int) text.getLocalBounds().width;
//    if (width > format.maxDimensions.x) {
//      size_t prev = std::string::npos;
//      float check = (width / maxWidth) + (width / maxWidth) * maxWidth;
//      size_t p;
//      while ((p = brokenString.find_last_of(' ', prev)) != std::string::npos) {
//        int charPos = (int) text.findCharacterPos(p).x;
//        if (charPos - check < 0) {
//          brokenString.insert(p, "\n");
//          check -= maxWidth;
//          lines++;
//          if (check < maxWidth) break;
//        }
//        prev = p - 1;
//      }
//      text.setString(brokenString);
//    }
//  }
//  yOffset += text.getLocalBounds().height; // or something

  const auto dims = drawBlock(anchor, string);
  if (format.horizontal == HorizontalAlign::Left)
    drawOffset.x += dims.x;
  if (format.horizontal == HorizontalAlign::Right)
    drawOffset.x -= dims.x;
}

void TextFrame::setColor(const unsigned char r,
                         const unsigned char g,
                         const unsigned char b) {
  setColor(sf::Color(r, g, b));
}

void TextFrame::setColor(const sf::Color &color) {
  this->color = color;
  colorSet = true;
}

void TextFrame::breakLine() {
  drawOffset.x = 0;

  switch (format.vertical) {
    case VerticalAlign::Top: {
      drawOffset.y += format.size;
    }
    case VerticalAlign::Middle:
      break;
    case VerticalAlign::Bottom: {
      drawOffset.y -= format.size;
    }
  }
}


}
