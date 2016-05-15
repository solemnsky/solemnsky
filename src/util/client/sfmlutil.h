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
/**
 * Utilities for client executables, depending on SFML window and graphics
 * modules.
 */
#ifndef SOLEMNSKY_UTIL_H
#define SOLEMNSKY_UTIL_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "util/types.h"

sf::Color mixColors(
    const sf::Color color1, const sf::Color color2, const float degree);

std::string printKey(const sf::Keyboard::Key key);

#endif //SOLEMNSKY_UTIL_H
