/**
 * Helper constructs to attach explicit meaning and utilities to floats.
 *
 * > float rotationControl{0}; // between -1 and 1
 * > rotationControl = clamp(-1, 1, newValue);
 *  vs
 * > Clamped{-1, 1, 0} rotationControl;
 * > rotationControl = newValue;
 */
#ifndef SOLEMNSKY_VALUE_H
#define SOLEMNSKY_VALUE_H

#include <vector>
#include <cmath>

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
  if (x > max) return min + std::fmod((x - min), (max - min));
  if (x < min) return max - cyclicClamp(min, max, (min - x));
  return x;
}

template<typename T>
T sign(const T x) {
  if (x > 0) return 1;
  if (x < 0) return -1;
  return 0;
}

template<typename T>
void approach(T &x, const T target, const T amount) {
  const T msign = sign(target - x);
  const T naive = x + msign * amount;
  if (sign(target - naive) != msign) {
    x = target;
    return;
  }
  x = naive;
}

/****
 * Cooldown values: floats that are used for a 'cooldown' effect, incrementing
 * up a range until they hit a maximum, at which point they assume a 'true'
 * conversion to bool. They can be cooled, reset, and primed.
 *
 * > Cooldown cooldown{1};
 * > print(cooldown.cool(0.5)); // 'false'
 * > print(cooldown.cool(0.6)); // 'true'
 * > cooldown.reset();
 * > print(cooldown); // 'false', cooldown now at 1
 */
class Cooldown {
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
 * Clamped values: floats that always snap back to a certain range when
 * assigned.
 *
 * > Clamped x{0, 1};
 * > x = 3;
 * > print(x) // '1' (nearest member of [0, 1] to 3)
 * > x -= 1.4;
 * > print(x) // '0'
 */
class Clamped {
private:
  float min, max;
  float value;

public:
  Clamped() = delete;

  Clamped(const float min, const float max);
  Clamped(const float min, const float max, const float value);
  Clamped &operator=(const float x);
  Clamped &operator+=(const float x);
  Clamped &operator-=(const float x);

  inline operator float() const { return value; }
};

/**
 * Cyclic values: floats that always cycle back to a certain range when
 * assigned.
 */
class Cyclic {
private:
  float min, max;
  float value;
public:

  Cyclic() = delete;

  Cyclic(const float min, const float max);
  Cyclic(const float min, const float max, const float value);
  Cyclic &operator=(const float x);
  Cyclic &operator+=(const float x);
  Cyclic &operator-=(const float x);

  inline operator float() const { return value; }
};

/**
 * Switched values: floats that may assume one of a set of values.
 *
 * > Switched x{{1, 2, 3}, 1};
 * > x = 1; // okay
 * > x = 4;
 * > (error printed, program execution terminated)
 */
class Switched {
private:
  std::vector<float> states;
  float value;

public:
  Switched() = delete;

  Switched(std::vector<float> states, float value) :
      states(states), value(value) { operator=(value); /* confirm validity */ };

  Switched &operator=(const float x);

  inline operator float() const { return value; }
};

/**
 * Angle value: represents an angle in degrees. Like a Cyclic but bound to
 * [0, 360[ by type. SFML doesn't seem to have this and I'm not going to be
 * using potentially redundant cyclicClamps everywhere...
 */
class Angle {
private:
  float value;
public:
  inline Angle() : Angle(0) { }
  Angle(const float x);

  Angle &operator=(const float x);
  Angle &operator+=(const float x);
  Angle &operator-=(const float x);

  inline operator float() const { return value; }
};

#endif //SOLEMNSKY_VALUE_H
