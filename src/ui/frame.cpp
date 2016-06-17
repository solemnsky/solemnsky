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
#include <assert.h>
#include <cmath>
#include "util/methods.hpp"
#include "frame.hpp"

namespace ui {

const sf::Color Frame::alphaScaleColor(const sf::Color color) {
  sf::Color newColor(color);
  newColor.a *= alphaStack.top();
  return newColor;
}

Frame::Frame(sf::RenderWindow &window) : window(window) {
  resize();
}

void Frame::resize() {
  sf::Vector2u size = window.getSize();
  float sx(size.x), sy(size.y);
  const float viewAspect = sx / sy;
  static constexpr float targetAspect = 16.0f / 9.0f;

  // set the view
  sf::View view;
  view.setCenter(800, 450);
  if (viewAspect > targetAspect) {
    view.setSize(1600 * (viewAspect / targetAspect), 900);
    float scalar = 900 / sy;
    windowToFrame = sf::Transform().scale(scalar, scalar);
    windowToFrame.translate(-(sx - (sy * targetAspect)) / 2, 0);
  }
  else {
    view.setSize(1600, 900 * (targetAspect / viewAspect));
    float scalar = 1600 / sx;
    windowToFrame = sf::Transform().scale(scalar, scalar);
    windowToFrame.translate(0, -(sy - (sx / targetAspect)) / 2);
  }

  window.setView(view);
}

void Frame::beginDraw() {
  primCount = 0;
  transformStack = std::stack<sf::Transform>({sf::Transform::Identity});
  alphaStack = std::stack<float>({1});
  window.clear(sf::Color::Black);
}

void Frame::endDraw() {
  sf::Vector2u size = window.getSize();
  sf::Vector2f topLeft = windowToFrame.transformPoint(sf::Vector2f(0, 0));
  sf::Vector2f bottomRight = windowToFrame.transformPoint(
      sf::Vector2f(size));

  // overwrite the margins outside of the 1600 x 900 we allow 'ctrl' to draw in
  sf::Color color = sf::Color(20, 20, 20, 255);
  drawRect(topLeft, sf::Vector2f(0, 900), color);
  drawRect(sf::Vector2f(1600, 0), bottomRight, color);
  drawRect(topLeft, sf::Vector2f(1600, 0), color);
  drawRect(sf::Vector2f(0, 900), bottomRight, color);
}

void Frame::pushTransform(const sf::Transform &transform) {
  auto newTransform = transformStack.top();
  transformStack.push(newTransform.combine(transform));
}

void Frame::popTransform() {
  assert(transformStack.size() > 1);
  transformStack.pop();
}

void Frame::withTransform(const sf::Transform &transform,
                          std::function<void()> fn) {
  pushTransform(transform);
  fn();
  popTransform();
}

void Frame::pushAlpha(const float alpha) {
  alphaStack.push(alphaStack.top() * alpha);
}

void Frame::popAlpha() {
  assert(alphaStack.size() > 1);
  alphaStack.pop();
}

void Frame::withAlpha(const float alpha, std::function<void()> fn) {
  pushAlpha(alpha);
  fn();
  popAlpha();
}

void Frame::withAlphaTransform(
    const float alpha, const sf::Transform &transform,
    std::function<void()> fn) {
  pushAlpha(alpha), pushTransform(transform);
  fn();
  popTransform(), popAlpha();
}

void Frame::drawCircle(const sf::Vector2f &pos,
                       const float radius,
                       const sf::Color &color) {
  primCount++;
  sf::CircleShape circle(radius);
  circle.setPosition(pos - sf::Vector2f(radius, radius));
  circle.setFillColor(alphaScaleColor(color));
  window.draw(circle, transformStack.top());
}

void Frame::drawRect(const sf::Vector2f &topLeft,
                     const sf::Vector2f &bottomRight,
                     const sf::Color &color) {
  primCount++;
  sf::RectangleShape rect(bottomRight - topLeft);
  rect.setPosition(topLeft);
  rect.setFillColor(alphaScaleColor(color));
  window.draw(rect, transformStack.top());
}

void Frame::drawPoly(const std::vector<sf::Vector2f> &vertices,
                     const sf::Color &color) {
  primCount++;
  sf::ConvexShape shape(vertices.size());
  size_t i = 0;
  for (const auto &vertex : vertices) {
    shape.setPoint(i, vertex);
    ++i;
  }
  shape.setFillColor(alphaScaleColor(color));
  window.draw(shape, transformStack.top());
}

void Frame::drawPolyOutline(const std::vector<sf::Vector2f> &vertices,
                            const sf::Color &color) {
  primCount++;
  std::vector<sf::Vertex> shape(vertices.size()+1);
  sf::Color col = alphaScaleColor(color);
  size_t i = 0;
  for (const auto &vertex : vertices) {
    shape[i] = sf::Vertex(vertex, col);
    ++i;
  }
  shape[vertices.size()] = sf::Vertex(vertices[0], col);
  window.draw(shape.data(),
              shape.size(),
              sf::PrimitiveType::LinesStrip,
              transformStack.top());
}

sf::Vector2f Frame::drawText(const sf::Vector2f &pos,
                             std::function<void(TextFrame &)> process,
                             const TextFormat &format,
                             const sf::Font &font) {
  TextFrame frame(*this, pos, format, font);
  process(frame);
  return frame.endRender();
}

void Frame::drawSprite(const sf::Texture &texture,
                       const sf::Vector2f &pos,
                       const sf::IntRect &portion) {
  primCount++;
  sf::Sprite sprite;
  sprite.setTexture(texture);
  sprite.setPosition(pos);
  sprite.setTextureRect(portion);
  sprite.setColor(sf::Color(255,
                            255,
                            255,
                            (sf::Uint8) (255 * alphaStack.top())));
  window.draw(sprite, transformStack.top());
}

}
