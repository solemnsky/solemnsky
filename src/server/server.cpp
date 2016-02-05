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

  sky::ServerMessage response;
  sky::ClientMessage message;
  tg::Packet transmitPacket;

  // send ping
  message.joke = "no joke for you";
  message.type = sky::ClientMessage::Type::Ping;
  transmitPacket.writeReset();
  tg::packInto(clientMessagePack, message, transmitPacket);
  telegraph.transmit(tg::Transmission(transmitPacket, "localhost", 4242,
                                      tg::Strategy()));

  bool running = true;
  while (running) {
    telegraph.receive([&](tg::Reception &&reception) {
      tg::unpackInto(clientMessagePack, reception.packet, message);
      switch (message.type) {
        case sky::ClientMessage::Type::Ping: {
          appLog(LogType::Info, "ping received!");
          appLog(LogType::Info, "joke is: " + message.joke);
          appLog(LogType::Info, "responding with pong");

          response.type = sky::ServerMessage::Type::Pong;
          response.motd.reset();
          transmitPacket.writeReset();
          tg::packInto(serverMessagePack, response, transmitPacket);

          telegraph.transmit(tg::Transmission(
              transmitPacket, reception.address, 4243, tg::Strategy()));
          break;
        }
        case sky::ClientMessage::Type::MotD: {
          appLog(LogType::Info, "MotD request received!");
          appLog(LogType::Info, "joke is: " + message.joke);
          appLog(LogType::Info, "responding with MotD");

          response.type = sky::ServerMessage::Type::MotD;
          response.motd = "Wheeeee I'm in a UDP packet look at me!";
          tg::packInto(serverMessagePack, response, transmitPacket);

          telegraph.transmit(tg::Transmission(
              transmitPacket, reception.address, 4243, tg::Strategy()));
          break;
        }
      }
    });
  }
}
