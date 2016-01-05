#include "value.h"
#include <cmath>
#include "util.h"

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