#include "protocol.h"

namespace sky {

/**
 * ClientPacket.
 */

std::string ClientPacket::dump() const {
  switch (type) {
    case Type::Ping:
      return "Ping";
    case Type::ReqJoin:
      return "ReqJoin";
    case Type::ReqDelta:
      return "ReqDelta";
    case Type::Chat:
      return "Chat";
  }
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ClientPacket, TYPE>(RULE, &ClientPacket::PTR)
ClientPacketPack::ClientPacketPack() :
    tg::ClassPack<ClientPacket>(
        member(ClientPacket::Type, type,
               tg::EnumPack<ClientPacket::Type>(3)),
        member(optional<std::string>, stringData, tg::optStringPack),
        member(optional<PlayerDelta>, playerDelta,
               tg::OptionalPack<PlayerDelta>(PlayerDeltaPack()))
    ) { }
#undef member

/**
 * ServerPacket.
 */

std::string ServerPacket::dump() const {
  switch (type) {
    case Type::Pong:
      return "Pong";
    case Type::AckJoin:
      return "AckJoin";
    case Type::NotifyDelta:
      return "NotifyDelta";
    case Type::NotifyMessage:
      return "NotifyMessage";
  }
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<ServerPacket, TYPE>(RULE, &ServerPacket::PTR)
ServerPacketPack::ServerPacketPack() :
    tg::ClassPack<ServerPacket>(
        member(ServerPacket::Type, type,
               tg::EnumPack<ServerPacket::Type>(3)),
        member(optional<std::string>, stringData, tg::optStringPack),
        member(optional<PID>, pid, tg::OptionalPack<PID>(pidPack)),
        member(optional<ArenaInitializer>, arenaInitializer,
               tg::OptionalPack<ArenaInitializer>(ArenaInitializerPack())),
        member(optional<sky::ArenaDelta>, arenaDelta,
               tg::OptionalPack<sky::ArenaDelta>(ArenaDeltaPack()))
    ) { }
#undef member

}