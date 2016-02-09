/**
 * Utilities for client executables, depending on SFML window and graphics
 * modules.
 */
#ifndef SOLEMNSKY_UTIL_H
#define SOLEMNSKY_UTIL_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "util/types.h"

/****
 * Do stuff with colors.
 */
sf::Color mixColors(
    const sf::Color color1, const sf::Color color2, const float degree);

#endif //SOLEMNSKY_UTIL_H
