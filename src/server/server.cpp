#include "server.h"

#include "telegraph/telegraph.h"
#include "sky/delta.h"
#include "sky/protocol.h"
#include <iostream>
#include "util/methods.h"

int main() {
  using sky::pk::clientMessagePack;
  using sky::pk::serverMessagePack;

  tg::Telegraph telegraph(4242);
  tg::Strategy basicStrategy = tg::Strategy(tg::Strategy::Control::None);

  sky::ServerMessage response;
  sky::ClientMessage message;
  tg::Packet transmitPacket;

  bool running = true;
  while (running) {
    telegraph.receive([&](tg::Reception &&reception) {
      message = tg::unpack(clientMessagePack, reception.packet);
      switch (message.type) {
        case sky::ClientMessage::Type::Ping: {
          appLog(LogType::Info, "ping received!");
          appLog(LogType::Info, "joke is: " + message.joke);
          appLog(LogType::Info, "responding with pong");

          response.type = sky::ServerMessage::Type::Pong;
          response.motd = {};
          tg::packInto(serverMessagePack, response, transmitPacket);

          telegraph.transmit(tg::Transmission(
              transmitPacket, reception.address, basicStrategy));
          break;
        }
        case sky::ClientMessage::Type::MotD: {
          appLog(LogType::Info, "MotD request received!");
          appLog(LogType::Info, "joke is: " + message.joke);
          appLog(LogType::Info, "responding with MotD");

          response.type = sky::ServerMessage::Type::MotD;
          response.motd = {"Wheeeee I'm in a UDP packet look at me!"};
          tg::packInto(serverMessagePack, response, transmitPacket);

          telegraph.transmit(tg::Transmission(
              transmitPacket, reception.address, basicStrategy));
          break;
        }
      }
    });
  }
}
