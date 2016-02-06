#define _USE_MATH_DEFINES // for M_PI
#include "types.h"
#include <cmath>
#include "methods.h"

/**
 * Cooldown.
 */
void Cooldown::reset() { cooldown = period; }

void Cooldown::prime() { cooldown = 0; }

bool Cooldown::cool(const float delta) {
  cooldown = std::max(0.0f, cooldown - delta);
  return cooldown == 0;
}

Cooldown::Cooldown(const float period) : period(period), cooldown(period) { }

/**
 * Clamped.
 */
Clamped::Clamped(const float min, const float max) :
    min(min), max(max), value(min) { }

Clamped::Clamped(const float min, const float max, const float value) :
    min(min), max(max), value(clamp(min, max, value)) { }

Clamped &Clamped::operator=(const float x) {
  value = clamp(min, max, x);
  return *this;
}

Clamped &Clamped::operator+=(const float x) {
  value = clamp(min, max, value + x);
  return *this;
}

Clamped &Clamped::operator-=(const float x) {
  value = clamp(min, max, value - x);
  return *this;
}

/**
 * Cyclic: this code could be combined with Clamped if I were using value
 * templates, which I'm not, because it would make me use more value
 * templates in other places in the code, which would be bad because ...
 * well, I'm still working on this rationalization.
 */
Cyclic::Cyclic(const float min, const float max) :
    min(min), max(max), value(min) { }

Cyclic::Cyclic(const float min, const float max, const float value) :
    min(min), max(max), value(cyclicClamp(min, max, value)) { }

Cyclic &Cyclic::operator=(const float x) {
  value = cyclicClamp(min, max, x);
  return *this;
}

Cyclic &Cyclic::operator+=(const float x) {
  value = cyclicClamp(min, max, value + x);
  return *this;
}

Cyclic &Cyclic::operator-=(const float x) {
  value = cyclicClamp(min, max, value - x);
  return *this;
}

bool cyclicApproach(Cyclic &x, const float target, const float amount) {
  const float distance = cyclicDistance(x, target);
  x += sign(distance) * std::min(amount, std::abs(distance));
  return amount < distance;
}

float cyclicDistance(const Cyclic x, const float y) {
  const float range = x.max - x.min;
  const float diffUp = Cyclic(0, x.max - x.min, y - x);
  return (diffUp < range / 2) ? diffUp : diffUp - range;
}

/**
 * Switched.
 */
Switched &Switched::operator=(const float x) {
  for (float possible : states)
    if (x == possible) {
      value = x;
      return *this;
    }

  throw std::logic_error("Bad assignment to Switched value!");
}

/**
 * Angle
 */
Angle::Angle(const float x) : value(cyclicClamp<float>(0, 360, x)) { }

Angle::Angle(const sf::Vector2f &vec) {
  operator=(toDeg((float) atan2(vec.y, vec.x)));
}

Angle &Angle::operator=(const float x) {
  value = cyclicClamp<float>(0, 360, x);
  return *this;
}

Angle &Angle::operator+=(const float x) {
  value = cyclicClamp<float>(0, 360, x + value);
  return *this;
}

Angle &Angle::operator-=(const float x) {
  value = cyclicClamp<float>(0, 360, x - value);
  return *this;
}

sf::Vector2f Angle::toVector() {
  const float rad = toRad(value);
  return sf::Vector2f((float) cos(rad), (float) sin(rad));
}
