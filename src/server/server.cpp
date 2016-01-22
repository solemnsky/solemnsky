#include "server.h"
#include "base/packer.h"

void runServer() {
  Packet packet = pack((PackRules<float> &&) PackFloat(), 5);
  packet.dump();
}
