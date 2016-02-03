#include "server.h"
#include "pack/pack.h"
#include "sky/delta.h"
#include <iostream>

int main() {
  using sky::pack::planeTuningPack;

  sky::PlaneTuning tuning;
  pk::Packet packet = pack(planeTuningPack, tuning);
  packet.dump();

}
