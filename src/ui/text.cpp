/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <cmath>
#include "text.hpp"
#include "util/methods.hpp"
#include "frame.hpp"

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
  throw enum_error();
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
  throw enum_error();
}

/**
 * TextFormat.
 */
TextFormat::TextFormat(
    const int size,
    const float maxWidth,
    const HorizontalAlign horizontal,
    const VerticalAlign vertical) :
    size(size),
    maxWidth(maxWidth),
    horizontal(horizontal),
    vertical(vertical) { }

/**
 * TextFrame.
 */

TextFrame::TextFrame(Frame &parent,
                     const sf::Vector2f &anchor,
                     const TextFormat &format,
                     const sf::Font &font) :
    parent(parent),
    anchor(anchor),
    format(format),
    font(font) { }

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

  text.setFillColor(parent.alphaScaleColor(color));
  parent.window.draw(text, parent.transformStack.top());
  return {bounds.width, float(format.size)};
}

void TextFrame::print(const std::string &string) {
  assert(colorSet);
  parent.primCount++;

  sf::Text text;
  text.setFont(font);
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
