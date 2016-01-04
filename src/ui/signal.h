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
 * Example:
 *
 * Define:
 * > Signal<None> clickSig;
 *
 * In a tick() somewhere on a button, the user clicks a mouse:
 * > clickSig += {};
 *
 * In an signalRead() somewhere on a GUI, clicks are processed:
 * > for (auto click : clickSig) { ... process click ... }
 *
 * After reading happens, the signals are cleared:
 * > clickSig.clear();
 */
template<typename T>
class Signal {
private:
  std::vector<T> queue;

public:
  Signal() : queue() { }

  inline void operator+=(const T x) { queue.push_back(x); }

  inline operator std::vector<T>() { return queue; }

  inline operator bool() { return queue.size() != 0; }

  inline void clear() { queue = {}; }
};

}

#endif //SOLEMNSKY_SIGNAL_H
