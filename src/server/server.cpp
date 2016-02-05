#include "server.h"

#include "telegraph/telegraph.h"
#include "sky/delta.h"
#include <iostream>
#include "util/methods.h"

int main() {
  using sky::pk::planeTuningPack;

  tg::Telegraph telegraph(4242);

  sky::PlaneTuning tuning;
  tuning.energy.recharge = 9999;
  tg::Packet packet = pack(planeTuningPack, tuning);
  packet.dump();
  tuning = {};
  telegraph.transmit(
      tg::Transmission(packet, sf::IpAddress("localhost"),
                       tg::Strategy(tg::Strategy::Control::None)));

  bool received = false;
  while (!received) {
    telegraph.receive([&](tg::Reception &&reception) {
      appLog(LogType::Debug, "packet recieved!");
      reception.packet.dump();
      tuning = unpack(planeTuningPack, reception.packet);
      received = true;
    });
  }

  std::cout << tuning.energy.recharge;
}
