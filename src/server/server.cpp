#include "server.h"
#include "pack/pack.h"
#include "sky/delta.h"
#include <iostream>

int main() {
  using sky::pack::planeTuningPack;

  sky::PlaneTuning tuning;
  tuning.energy.recharge = 9999;
  pk::Packet packet = pack(planeTuningPack, tuning);
  packet.dump();
  tuning = unpack(planeTuningPack, packet);
  std::cout << tuning.energy.recharge;
}
