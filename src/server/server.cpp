#include "server.h"

#include "pack/telegraphy.h"
#include "sky/delta.h"
#include <iostream>
#include "util/methods.h"

int main() {
  using sky::pack::planeTuningPack;

  pk::Telegraph telegraph(4242);

  sky::PlaneTuning tuning;
  tuning.energy.recharge = 9999;
  pk::Packet packet = pack(planeTuningPack, tuning);
  packet.dump();
  tuning = {};
  telegraph.transmit(
      pk::Transmission(packet, sf::IpAddress("localhost"),
      pk::Strategy(pk::Strategy::Control::None)));

  bool received = false;
  while (!received) {
    telegraph.receive([&](pk::Reception &&reception) {
      appLog(LogType::Debug, "packet recieved!");
      reception.packet.dump();
      tuning = unpack(planeTuningPack, reception.packet);
      received = true;
    });
  }

  std::cout << tuning.energy.recharge;
}
