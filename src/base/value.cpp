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

  appLog(LogType::Error, "Bad assignment to Switched value!");
  assert(false); // no need for exceptions, this is practically a syntax error
}