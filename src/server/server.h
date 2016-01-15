#ifndef SOLEMNSKY_SERVER_H
#define SOLEMNSKY_SERVER_H

#include "sky/sky.h"

void runServer() {
  sky::Sky sky({1600, 900});
  // a client joins
  sky.joinPlane(0, sky::PlaneTuning());
  // some time passes
  sky.tick(0.6);
  // we want to get information from the sky
  appLog(
      LogType::Debug,
      "player is " + ((bool) sky.getPlane(0)->state) ? "alive" : "dead");
}

#endif //SOLEMNSKY_SERVER_H
