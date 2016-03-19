#include "protocol.h"
#include "util/methods.h"

namespace sky {

/**
 * ClientPacket.
 */

ClientPacket::ClientPacket() { }
ClientPacket::ClientPacket(
    const Type type,
    const optional<std::string> &stringData,
    const optional<PlayerDelta> &playerDelta,
    const optional<SkyDelta> &skyDelta) :
    type(type),
    stringData(stringData),
    playerDelta(playerDelta),
    skyDelta(skyDelta) { }

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
  return ClientPacket(ClientPacket::Type::ReqJoin, nickname);
}

ClientPacket ClientPacket::ReqPlayerDelta(const PlayerDelta &playerDelta) {
  return ClientPacket(ClientPacket::Type::ReqPlayerDelta, {}, playerDelta);
}

ClientPacket ClientPacket::ReqSkyDelta(const SkyDelta &skyDelta) {
  return ClientPacket(ClientPacket::Type::ReqSkyDelta, {}, {}, skyDelta);
}

ClientPacket ClientPacket::ReqSpawn() {
  return ClientPacket(ClientPacket::Type::ReqSpawn);
}

ClientPacket ClientPacket::ReqKill() {
  return ClientPacket(ClientPacket::Type::ReqKill);
}

ClientPacket ClientPacket::Chat(const std::string &message) {
  return ClientPacket(ClientPacket::Type::Chat, message);
}

/**
 * ServerMessage.
 */

ServerMessage::ServerMessage(const ServerMessage::Type type,
                             std::string contents,
                             optional<PID> from) :
    type(type),
    contents(contents),
    from(from) { }

ServerMessage::ServerMessage() { }

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
  return ServerMessage(ServerMessage::Type::Chat, contents, from);
}

ServerMessage ServerMessage::Broadcast(const std::string &contents) {
  return ServerMessage(ServerMessage::Type::Broadcast, contents);
}

/**
 * ServerPacket.
 */

ServerPacket::ServerPacket(
    const ServerPacket::Type type,
    const optional<ServerMessage> &message,
    const optional<PID> &pid,
    const optional<ArenaInitializer> &init,
    const optional<ArenaDelta> &delta) :
    type(type),
    message(message),
    pid(pid),
    init(init),
    delta(delta) { }

ServerPacket::ServerPacket() { }

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
  return ServerPacket(ServerPacket::Type::Message, message);
}

ServerPacket ServerPacket::Init(const PID pid, const ArenaInitializer &init) {
  return ServerPacket(ServerPacket::Type::Init, {}, pid, init);
}

ServerPacket ServerPacket::Delta(const ArenaDelta &delta) {
  return ServerPacket(ServerPacket::Type::Delta, {}, {}, {}, delta);
}

}