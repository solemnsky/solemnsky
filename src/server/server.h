#ifndef SOLEMNSKY_SERVER_H
#define SOLEMNSKY_SERVER_H

#include "sky/sky.h"
#include "sky/arena.h"
#include "telegraph/telegraph.h"
#include "sky/delta.h"
#include "sky/protocol.h"
#include "sky/arena.h"
#include <iostream>

class Server {
 private:
  double uptime;

  optional<sky::Sky> sky;
  sky::Arena arena;

 public:
  Server();

  void tick(float delta);

  bool running;
};

int main();

#endif //SOLEMNSKY_SERVER_H
