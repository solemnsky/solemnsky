#include "protocol.h"

namespace sky {

/**
 * ClientPacket.
 */

ClientPacket::ClientPacket() { }
ClientPacket::ClientPacket(
    const Type type,
    const optional<std::string> &stringData,
    const optional<PlayerDelta> &playerDelta,
    const optional<SkyDelta> &skyDelta,
    const optional<Team> team) :
    type(type),
    stringData(stringData),
    playerDelta(playerDelta),
    skyDelta(skyDelta),
    team(team) { }

bool ClientPacket::verifyStructure() const {
  switch (type) {
    case Type::Ping:
      return true;
    case Type::ReqJoin:
      return verifyFields(stringData);
    case Type::ReqDelta:
      return verifyFields(playerDelta);
    case Type::ReqTeamChange:
      return verifyFields(team);
    case Type::ReqSpawn:
      return true;
    case Type::ReqKill:
      return true;
    case Type::NoteSkyDelta:
      return verifyFields(skyDelta);
    case Type::Chat:
      return verifyFields(stringData);
  }
}

ClientPacket ClientPacket::Ping() {
  return ClientPacket(ClientPacket::Type::Ping);
}

ClientPacket ClientPacket::ReqJoin(const std::string &nickname) {
  return ClientPacket(ClientPacket::Type::ReqJoin, nickname);
}

ClientPacket ClientPacket::ReqDelta(const PlayerDelta &playerDelta) {
  return ClientPacket(ClientPacket::Type::ReqDelta, {}, playerDelta);
}

ClientPacket ClientPacket::ReqTeamChange(const Team team) {
  return ClientPacket(ClientPacket::Type::ReqTeamChange,
                      {}, {}, {}, team);
}

ClientPacket ClientPacket::ReqSpawn() {
  return ClientPacket(ClientPacket::Type::ReqSpawn);
}

ClientPacket ClientPacket::ReqKill() {
  return ClientPacket(ClientPacket::Type::ReqKill);
}

ClientPacket ClientPacket::NoteSkyDelta(const SkyDelta &skyDelta) {
  return ClientPacket(ClientPacket::Type::NoteSkyDelta, {}, {}, skyDelta);
}

ClientPacket ClientPacket::Chat(const std::string &message) {
  return ClientPacket(ClientPacket::Type::Chat, message);
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ClientPacket, TYPE>(RULE, &ClientPacket::PTR)
ClientPacketPack::ClientPacketPack() :
    tg::ClassPack<ClientPacket>(
        member(ClientPacket::Type, type,
               tg::EnumPack<ClientPacket::Type>(3)),
        member(optional<std::string>, stringData, tg::optStringPack),
        member(optional<PlayerDelta>, playerDelta,
               tg::OptionalPack<PlayerDelta>(PlayerDeltaPack())),
        member(optional<SkyDelta>, skyDelta,
               tg::OptionalPack<SkyDelta>(SkyDeltaPack()))
    ) { }
#undef member

/**
 * ServerMessage.
 */

ServerMessage::ServerMessage(const ServerMessage::Type type,
                             std::string contents,
                             optional<std::string> from) :
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
}

ServerMessage ServerMessage::Chat(const std::string &from,
                                  const std::string &contents) {
  return ServerMessage(ServerMessage::Type::Chat, contents, from);
}

ServerMessage ServerMessage::Broadcast(const std::string &contents) {
  return ServerMessage(ServerMessage::Type::Broadcast, contents);
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ServerMessage, TYPE>(RULE, &ServerMessage::PTR)
ServerMessagePack::ServerMessagePack() :
    tg::ClassPack<ServerMessage>(
        member(ServerMessage::Type, type,
               tg::EnumPack<ServerMessage::Type>(1)),
        member(optional<std::string>, from, tg::optStringPack),
        member(std::string, contents, tg::stringPack)
    ) { }
#undef member

/**
 * ServerPacket.
 */

ServerPacket::ServerPacket(
    const ServerPacket::Type type,
    const optional<ServerMessage> &message,
    const optional<PID> &pid,
    const optional<ArenaInitializer> &arenaInitializer,
    const optional<ArenaDelta> &arenaDelta,
    const optional<SkyDelta> &skyDelta) :
    type(type),
    message(message),
    pid(pid),
    arenaInitializer(arenaInitializer),
    arenaDelta(arenaDelta),
    skyDelta(skyDelta) { }

ServerPacket::ServerPacket() { }

bool ServerPacket::verifyStructure() const {
  switch (type) {
    case Type::Pong:
      return true;
    case Type::Message:
      return verifyFields(message);
    case Type::AckJoin:
      return verifyFields(pid, arenaInitializer);
    case Type::NoteArenaDelta:
      return verifyFields(arenaDelta);
    case Type::NoteSkyDelta:
      return verifyFields(skyDelta);
  }
}

ServerPacket ServerPacket::Pong() {
  return ServerPacket(ServerPacket::Type::Pong);
}

ServerPacket ServerPacket::Message(const ServerMessage &message) {
  return ServerPacket(ServerPacket::Type::Message, message);
}

ServerPacket ServerPacket::AckJoin(const PID pid,
                                   const ArenaInitializer &arenaInitializer) {
  return ServerPacket(ServerPacket::Type::AckJoin, {}, pid, arenaInitializer);
}

ServerPacket ServerPacket::NoteArenaDelta(const ArenaDelta &arenaDelta) {
  return ServerPacket(
      ServerPacket::Type::NoteArenaDelta, {}, {}, {}, arenaDelta);
}

ServerPacket ServerPacket::NoteSkyDelta(const SkyDelta &skyDelta) {
  return sky::ServerPacket(
      ServerPacket::Type::NoteSkyDelta, {}, {}, {}, {}, skyDelta);
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ServerPacket, TYPE>(RULE, &ServerPacket::PTR)
ServerPacketPack::ServerPacketPack() :
    tg::ClassPack<ServerPacket>(
        member(ServerPacket::Type, type, tg::EnumPack<ServerPacket::Type>(3)),
        member(optional<ServerMessage>, message,
               tg::OptionalPack<ServerMessage>(ServerMessagePack())),
        member(optional<PID>, pid, tg::OptionalPack<PID>(pidPack)),
        member(optional<ArenaInitializer>, arenaInitializer,
               tg::OptionalPack<ArenaInitializer>(ArenaInitializerPack())),
        member(optional<ArenaDelta>, arenaDelta,
               tg::OptionalPack<ArenaDelta>(ArenaDeltaPack())),
        member(optional<SkyDelta>, skyDelta,
               tg::OptionalPack<SkyDelta>(SkyDeltaPack()))
    ) { }
#undef member

}