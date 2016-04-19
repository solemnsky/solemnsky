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
    case Type::ReqAction:
      return verifyFields(action, state);
    case Type::ReqSpawn:
      return true;
    case Type::ReqKill:
      return true;
    case Type::Chat:
      return verifyFields(stringData);
    case Type::RCon:
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

ClientPacket ClientPacket::ReqAction(const Action &action,
                                     const bool state) {
  ClientPacket packet(Type::ReqAction);
  packet.action = action;
  packet.state = state;
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

ClientPacket ClientPacket::RCon(const std::string &command) {
  ClientPacket packet(Type::RCon);
  packet.stringData = command;
  return packet;
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
    case Type::Init:
      return verifyFields(pid, arenaInit, skyInit);
    case Type::DeltaArena:
      return verifyFields(arenaDelta);
    case Type::DeltaSky:
      return verifyFields(skyDelta);
    case Type::Chat:
      return verifyFields(pid, stringData);
    case Type::Broadcast:
      return verifyFields(stringData);
    case Type::RCon:
      return verifyFields(stringData);
  }
  return false;
}

ServerPacket ServerPacket::Pong() {
  return ServerPacket(ServerPacket::Type::Pong);
}

ServerPacket ServerPacket::Init(const PID pid,
                                const ArenaInitializer &arenaInit,
                                const SkyInitializer &skyInit) {
  ServerPacket packet(Type::Init);
  packet.pid = pid;
  packet.arenaInit = arenaInit;
  packet.skyInit = skyInit;
  return packet;
}

ServerPacket ServerPacket::DeltaArena(const ArenaDelta &arenaDelta) {
  ServerPacket packet(Type::DeltaArena);
  packet.arenaDelta = arenaDelta;
  return packet;
}

ServerPacket ServerPacket::DeltaSky(const SkyDelta &skyDelta) {
  ServerPacket packet(Type::DeltaSky);
  packet.skyDelta = skyDelta;
  return packet;
}

ServerPacket ServerPacket::Chat(const PID pid, const std::string &chat) {
  ServerPacket packet(Type::Chat);
  packet.pid = pid;
  packet.stringData = chat;
  return packet;
}

ServerPacket ServerPacket::Broadcast(const std::string &broadcast) {
  ServerPacket packet(Type::Broadcast);
  packet.stringData = broadcast;
  return packet;
}

ServerPacket ServerPacket::RCon(const std::string &message) {
  ServerPacket packet(Type::RCon);
  packet.stringData = message;
  return packet;
}

}