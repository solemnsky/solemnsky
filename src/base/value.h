/**
 * Beyond primitives like floats and bools and containers like vectors, there
 * are a series of increasingly complex plain-data values that appear in our
 * game. Certain useful constructs have been made to deal with them here,
 * providing safety and explicit meaning in the code that uses them.
 */
#ifndef SOLEMNSKY_VALUE_H
#define SOLEMNSKY_VALUE_H

#include <vector>

/****
 * Cooldown values: floats that are used for a 'cooldown' effect, incrementing
 * up a range until they cycle back to 0, at which point some event might occur.
 *
 * > Ticker ticker{1};
 * > ticker.tick(0.5);
 * > print(ticker); // 'false', not active yet
 * > ticker.tick(0.6); // this returns true
 * > print(ticker); // 'true', active until it's ticked again
 * > print(ticker.cooldown); // 1
 */
class Cooldown {
private:
  bool active = false;

public:
  float cooldown;
  float period;

  Cooldown(float period) : period(period), cooldown(period) { }

  bool tick(float delta);
  void prime();
  void wait();
  operator bool();
};

/**
 * clamp function
 */
template<typename T>
T clamp(const T min, const T max, const T x) {
  if (x > max) return max;
  if (x < min) return min;
  return x;
}

/**
 * Clamped values: floats that must stay in a certain range.
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

  Clamped(float min, float max, float value = 0) :
      min(min), max(max), value(clamp(min, max, value)) { }

  inline Clamped &operator=(float x) { value = clamp(min, max, x); }

  inline Clamped &operator+=(float x) { value = clamp(min, max, value + x); }

  inline Clamped &operator-=(float x) { value = clamp(min, max, value - x); }

  inline operator float() const { return value; }
};

/**
 * Switched values: floats that may assume any of a set of values.
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
  Switched(std::vector<float> states, float value) :
      states(states), value(value) { operator=(value); /* confirm validity */ };

  Switched & operator=(const float x);

  inline operator float() const { return value; }
};

#endif //SOLEMNSKY_VALUE_H
