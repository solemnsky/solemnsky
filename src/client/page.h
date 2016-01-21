/**
 * One of our UI pages. It helps to be able to have a few generalizations
 * about them.
 */
#ifndef SOLEMNSKY_PAGE_H
#define SOLEMNSKY_PAGE_H

#include "ui/control.h"

class Page : public ui::Control {
public:
  virtual void reset() { } // reset all user interface elements (don't leave
  // buttons hot, for example)

  // TODO: other things, potentially
};

#endif //SOLEMNSKY_PAGE_H
