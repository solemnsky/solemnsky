/**
 * Utilities in the form of types, useful for declarations.
 *
 * Everything here is covered by unit testing; see the respective tests for
 * usage example.
 */
#ifndef SOLEMNSKY_VALUE_H
#define SOLEMNSKY_VALUE_H

#include <vector>
#include <ratio>
#include <cmath>
#include <SFML/System.hpp>
#include <boost/optional.hpp>

/****
 * Optional.
 */

using boost::optional;

/**
 * Useful functions.
 */
template<typename T>
T clamp(const T min, const T max, const T x) {
  if (x > max) return max;
  if (x < min) return min;
  return x;
}

template<typename T>
T cyclicClamp(const T min, const T max, const T x) {
  if (x > max) return min + std::fmod(x - min, max - min);
  if (x < min) return max - cyclicClamp(min, max, (min - x));
  if (x == max) return min;
  return x;
}

template<typename T>
T sign(const T x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

/**
 * Approach a value at a linear rate, potentially hitting and staying at it.
 */
template<typename T, typename G>
bool approach(T &x, const G target, const G amount) {
  // having a different type variables for the reference is useful when it's
  // one of the containers we define below; approach(Clamped &, const float,
  // const float)  works, for instance.
  const G msign = sign(target - x);
  const G naive = x + msign * amount;
  if (sign(target - naive) != msign) {
    x = target;
    return true;
  }
  x = naive;
  return false;
}

template<typename T>
bool approach(T &x, const T target, const T amount) {
  return approach<T, T>(x, target, amount);
}

/****
 * Float-augmentation types.
 * Below we have a series of types that build a wrapper around a float,
 * constricting the way in which it can be used.
 */

/**
 * Floats that are used for a 'cooldown' effect.
 */
struct Cooldown {
 public:
  float cooldown;
  float period;

  Cooldown(const float period);
  bool cool(const float delta);
  void reset();
  void prime();;

  inline operator bool() { return cooldown == 0; }
};

/**
 * Float in the [0, 1] range.
 */
struct Clamped {
 private:
  float value;

 public:
  Clamped();
  Clamped(const float value);
  Clamped &operator=(const float x);
  Clamped &operator+=(const float x);
  Clamped &operator-=(const float x);

  inline operator float() const { return value; }
};

/**
 * Floats that always cycle back to a certain range when assigned. ([min, max[)
 */
struct Cyclic {
 private:
  float value;
 public:
  float min, max;

  Cyclic() = delete;

  Cyclic(const float min, const float max);
  Cyclic(const float min, const float max, const float value);
  Cyclic &operator=(const float x);
  Cyclic &operator+=(const float x);
  Cyclic &operator-=(const float x);

  inline operator float() const { return value; }
};

bool cyclicApproach(Cyclic &x, const float target, const float amount);
float cyclicDistance(const Cyclic x, const float y);

/**
 * The movement of a one-dimensional variable at a constant or null rate.
 */

enum class Movement {
  Up, Down, None
};

float movementValue(const Movement movement);

/**
 * Angle value: represents an angle in degrees.
 */
struct Angle {
 private:
  Cyclic value;
 public:
  inline Angle() : Angle(0) { }
  Angle(const float x);
  Angle(const sf::Vector2f &);

  Angle &operator=(const float x);
  Angle &operator+=(const float x);
  Angle &operator-=(const float x);

  sf::Vector2f toVector();

  inline operator float() const { return value; }
};

/**
 * Some types have certain invariant structures that can be violated in
 * instances received from the network. This is a tidy way to verify
 * these invariants (for instance, our ad-hoc sum types in protocol.h).
 */

class VerifyStructure {
 public:
  virtual bool verifyStructure() const = 0;
};

#endif //SOLEMNSKY_VALUE_H
