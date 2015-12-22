/**
 * Generally helpful utilities.
 */
#ifndef SOLEMNSKY_UTIL_H
#define SOLEMNSKY_UTIL_H

#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

/**
 * Manages an action that becomes momentarily active on regular intervals. (The 'tick' of an analog clock is the analogy
 * that inspires its name.)
 */
class Ticker {
private:
    bool active = false;

public:
    float cooldown;
    float period;
    Ticker(float period) : period(period), cooldown(period) { }

    bool tick(float delta);
    void prime();
    void wait();

    operator bool();
};

/**
 * Convert between box2d and SFML types.
 */
class FromBox2d {
public:
    static sf::Vector2f vector(b2Vec2 vec);
};

#endif //SOLEMNSKY_UTIL_H
