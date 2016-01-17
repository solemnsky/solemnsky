/**
 * Utilities for client executables, dependening on SFML window and graphics
 * modules.
 */
#ifndef SOLEMNSKY_SRV_UTIL_H
#define SOLEMNSKY_SRV_UTIL_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "util.h"

/****
 * Extract useful information from sf::Events.
 */

optional<bool> getMouseButtonAction(sf::Event event);

/****
 * Do stuff with colors.
 */
sf::Color mixColors(
    const sf::Color color1, const sf::Color color2, const float degree);


#endif //SOLEMNSKY_SRV_UTIL_H
