#include "server.h"

#include "pack/telegraphy.h"
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

  pk::Telegraph telegraph(424242);
  telegraph.transmit(
      pk::Transmission(packet, sf::IpAddress("192.134.329"),
      pk::Strategy(pk::Strategy::Control::None)));
  telegraph.receive([&](pk::Reception &&reception) {
    tuning = unpack(planeTuningPack, reception.packet);
  });
}
