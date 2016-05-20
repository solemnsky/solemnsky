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
#include "types.h"
#define _USE_MATH_DEFINES // for M_PI
#include <cmath>
#include <numeric>
#include "methods.h"

/**
 * RollingSampler.
 */

RollingSampler::RollingSampler(const unsigned int maxMemory) :
    maxMemory(maxMemory) {
  push(0);
}

void RollingSampler::push(const float value) {
  if (data.size() >= maxMemory) {
    data.erase(data.begin());
  }
  data.push_back(value);
}

float RollingSampler::mean() const {
  if (data.size() == 0) return 0;
  return std::accumulate(data.begin(), data.end(), 0.0f, std::plus<float>())
      / float(data.size());
}

float RollingSampler::max() const {
  if (data.size() == 0) return 0;
  return *std::max_element(data.begin(), data.end());
}

float RollingSampler::min() const {
  if (data.size() == 0) return 0;
  return *std::min_element(data.begin(), data.end());
}

SamplerSnapshot::SamplerSnapshot() :
    min(0), mean(0), max(0) { }

SamplerSnapshot::SamplerSnapshot(const RollingSampler &sampler) :
    min(sampler.min()), mean(sampler.mean()), max(sampler.max()) { }

std::string printMs(float ms) {
  return std::to_string(int(std::round(ms * 1000)));
}

std::string SamplerSnapshot::print() const {
  return printMs(min) + ";" + printMs(mean) + ";" + printMs(max);
}

/**
 * Cooldown.
 */

void Cooldown::reset() { cooldown = period; }

void Cooldown::prime() { cooldown = 0; }

bool Cooldown::cool(const float delta) {
  cooldown = std::max(0.0f, cooldown - delta);
  return cooldown == 0;
}

Cooldown::Cooldown(const float period) :
    cooldown(period), period(period) { }

/**
 * Clamped.
 */

Clamped::Clamped() : value(0) { }

Clamped::Clamped(const float value) :
    value(clamp(0.0f, 1.0f, value)) { }

Clamped &Clamped::operator=(const float x) {
  return operator=(Clamped(x));
}

Clamped &Clamped::operator+=(const float x) {
  value = clamp(0.0f, 1.0f, value + x);
  return *this;
}

Clamped &Clamped::operator-=(const float x) {
  value = clamp(0.0f, 1.0f, value - x);
  return *this;
}

/**
 * Movement.
 */
float movementValue(const Movement movement) {
  switch (movement) {
    case Movement::Up:
      return 1;
    case Movement::Down:
      return -1;
    case Movement::None:
      return 0;
  }
  throw std::logic_error("bad enum");
}

/**
 * Cyclic.
 */

Cyclic::Cyclic(const float min, const float max) :
    value(min), min(min), max(max) { }

Cyclic::Cyclic(const float min, const float max, const float value) :
    value(cyclicClamp(min, max, value)), min(min), max(max) { }

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
 * Angle.
 */

Angle::Angle(const float x) : value(0, 360, x) { }

Angle::Angle(const sf::Vector2f &vec) :
    Angle(toDeg((float) atan2(vec.y, vec.x))) { }

Angle &Angle::operator=(const float x) {
  this->value = x;
  return *this;
}

Angle &Angle::operator+=(const float x) {
  this->value += x;
  return *this;
}

Angle &Angle::operator-=(const float x) {
  this->value -= x;
  return *this;
}

sf::Vector2f Angle::toVector() {
  const float rad = toRad(value);
  return sf::Vector2f((float) cos(rad), (float) sin(rad));
}
Movement addMovement(const bool down, const bool up) {
  if (up == down) return Movement::None;
  if (up) return Movement::Up;
  return Movement::Down;
}
