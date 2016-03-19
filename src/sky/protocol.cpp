#include "protocol.h"
#include "util/methods.h"

namespace sky {

/**
 * ClientPacket.
 */

ClientPacket::ClientPacket() { }
ClientPacket::ClientPacket(const Type type) : type(type) { }

bool ClientPacket::verifyStructure() const {
  switch (type) {
    case Type::Ping:
      return true;
    case Type::ReqJoin:
      return verifyFields(stringData);
    case Type::ReqPlayerDelta:
      return verifyFields(playerDelta);
    case Type::ReqSkyDelta:
      return verifyFields(skyDelta);
    case Type::ReqSpawn:
      return true;
    case Type::ReqKill:
      return true;
    case Type::Chat:
      return verifyFields(stringData);
  }
  return false;
}

ClientPacket ClientPacket::Ping() {
  return ClientPacket(ClientPacket::Type::Ping);
}

ClientPacket ClientPacket::ReqJoin(const std::string &nickname) {
  ClientPacket packet(Type::ReqJoin);
  packet.stringData = nickname;
  return packet;
}

ClientPacket ClientPacket::ReqPlayerDelta(const PlayerDelta &playerDelta) {
  ClientPacket packet(Type::ReqPlayerDelta);
  packet.playerDelta = playerDelta;
  return packet;
}

ClientPacket ClientPacket::ReqSkyDelta(const SkyDelta &skyDelta) {
  ClientPacket packet(Type::ReqSkyDelta);
  packet.skyDelta = skyDelta;
  return packet;
}

ClientPacket ClientPacket::ReqSpawn() {
  return ClientPacket(ClientPacket::Type::ReqSpawn);
}

ClientPacket ClientPacket::ReqKill() {
  return ClientPacket(ClientPacket::Type::ReqKill);
}

ClientPacket ClientPacket::Chat(const std::string &message) {
  ClientPacket packet(Type::Chat);
  packet.stringData = message;
  return packet;
}

/**
 * ServerMessage.
 */

ServerMessage::ServerMessage() { }

ServerMessage::ServerMessage(const Type type) : type(type) { }

bool ServerMessage::verifyStructure() const {
  switch (type) {
    case Type::Chat:
      return verifyFields(from);
    case Type::Broadcast:
      return true;
  }
  return false;
}

ServerMessage ServerMessage::Chat(const PID &from,
                                  const std::string &contents) {
  ServerMessage message(Type::Chat);
  message.from = from;
  message.contents = contents;
  return message;
}

ServerMessage ServerMessage::Broadcast(const std::string &contents) {
  ServerMessage message(Type::Broadcast);
  message.contents = contents;
  return message;
}

/**
 * ServerPacket.
 */

ServerPacket::ServerPacket() { }

ServerPacket::ServerPacket(const Type type) : type(type) { }

bool ServerPacket::verifyStructure() const {
  switch (type) {
    case Type::Pong:
      return true;
    case Type::Message: {
      return verifyFields(message);
    }
    case Type::Init:
      return verifyFields(pid, init);
    case Type::Delta:
      return verifyFields(delta);
  }
  return false;
}

ServerPacket ServerPacket::Pong() {
  return ServerPacket(ServerPacket::Type::Pong);
}

ServerPacket ServerPacket::Message(const ServerMessage &message) {
  ServerPacket packet(Type::Message);
  packet.message = message;
  return packet;
}

ServerPacket ServerPacket::Init(const PID pid, const ArenaInitializer &init) {
  ServerPacket packet(Type::Init);
  packet.pid = pid;
  packet.init = init;
  return packet;
}

ServerPacket ServerPacket::Delta(const ArenaDelta &delta) {
  ServerPacket packet(Type::Delta);
  packet.delta = delta;
  return packet;
}

}