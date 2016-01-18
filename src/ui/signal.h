/**
 * Certain UI elements want to transmit 'signals', logical events that are
 * processed exactly once by every signal-listening entity in the system and
 * can transmit some information.
 * Here we describe the centerpiece of our basic signal management framework.
 */
#ifndef SOLEMNSKY_SIGNAL_H
#define SOLEMNSKY_SIGNAL_H

#include "base/base.h"
#include <vector>

namespace ui {

/**
 * So Signal<None> can be constructed.
 */
struct None {
  None() { }
};

/**
 * The idea of a Signal.
 */
template<typename T>
using Signal = std::vector<T>;

}

#endif //SOLEMNSKY_SIGNAL_H
