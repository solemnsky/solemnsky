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
#include "shape.hpp"
#include "util/methods.hpp"

namespace sky {

/**
 * Shape.
 */

Shape::Shape(const Shape::Type type) :
    type(type) { }

Shape sky::Shape::Circle(const float radius) {
  sky::Shape shape(Type::Circle);
  shape.radius = radius;
  return shape;
}

Shape sky::Shape::Polygon(const std::vector<sf::Vector2f> &vertices) {
  sky::Shape shape(Type::Polygon);
  shape.vertices = vertices;
  return shape;
}

Shape sky::Shape::Rectangle(const sf::Vector2f &dimensions) {
  sky::Shape shape(Type::Rectangle);
  shape.dimensions = dimensions;
  return shape;
}

}

