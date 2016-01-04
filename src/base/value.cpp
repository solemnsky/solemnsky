#include "value.h"

/**
 * Cooldown.
 */

bool Cooldown::tick(float delta) {
  cooldown += delta;
  float newCooldown = std::fmod(cooldown, period);
  active = newCooldown != cooldown;
  cooldown = newCooldown;
  return active;
}

void Cooldown::prime() {
  cooldown = period;
}

void Cooldown::wait() {
  cooldown = 0;
}

Cooldown::operator bool() { return active; }

/**
 * Switched.
 */

Switched &Switched::operator=(const float x) {
  for (float possible : states)
    if (x == possible) {
      value = x;
      return *this;
    }

  appLog(LogType::Error, "Bad assignment to Switched value! This error "
      "should never pass into release.");
  assert(false); // bad value
}