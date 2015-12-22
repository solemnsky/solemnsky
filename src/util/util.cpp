#include <cmath>
#include "util.h"

bool Ticker::tick(float delta) {
    cooldown += delta;
    float newCooldown = std::fmod(cooldown, period);
    active = newCooldown != cooldown;
    cooldown = newCooldown;
    return active;
}

void Ticker::prime() {
    cooldown = period;
}

void Ticker::wait() {
    cooldown = 0;
}

Ticker::operator bool() { return active; }

sf::Vector2f FromBox2d::vector(b2Vec2 vec) {
    return sf::Vector2f(vec.x, vec.y);
}
