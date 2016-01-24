#include "server.h"
#include "util/packer.h"

int main() {
  Packet packet = pack((PackRules<float> &&) PackFloat(), 5);
  packet.dump();
}
