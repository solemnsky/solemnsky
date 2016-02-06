#include "server.h"

#include "telegraph/telegraph.h"
#include "sky/delta.h"
#include "sky/protocol.h"
#include <iostream>
#include "util/methods.h"

int main() {
  using sky::pk::clientPacketPack;
  using sky::pk::serverPacketPack;
  using namespace sky::prot;

  tg::Telegraph<ServerPacket, ClientPacket> telegraph(
      4242, serverPacketPack, clientPacketPack);

  bool running = true;
  while (running) {
    telegraph.receive([&](tg::Reception<ClientPacket> &&reception) {
      switch (reception.value.type) {
        case ClientPacket::Type::Ping: {
          appLog(LogType::Info, "ping request recieved.");
          appLog(LogType::Info, "responding with pong.");
          telegraph.transmit(ServerPong(), reception.address, 4243);
          break;
        }
        case ClientPacket::Type::MotD: {
          appLog(LogType::Info, "MotD request received.");
          appLog(LogType::Info, "responding with MotD.");
          telegraph.transmit(
              ServerMotD(
                  std::string("Wheeeee I'm in a UDP packet look at me!")),
              reception.address, 4243);
          break;
        }
        case ClientPacket::Type::Chat: {
          appLog(LogType::Info, "chat packet received.");
          appLog(LogType::Info, ">>" + *reception.value.stringData);
          break;
        }
      }
    });
  }
}
