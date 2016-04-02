/**
 * Widget used to select a keyboard key.
 */
#pragma once
#include "ui/control.h"

namespace ui {

class KeySelector: public ui::Control {
 public:
  struct Style {
    Style() { }
  } style;
 private:

 public:
  KeySelector() = default;
};

}
