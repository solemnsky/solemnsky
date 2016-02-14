#include "protocol.h"

namespace sky {
namespace prot {

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
  tg::MemberRule<prot::ClientPacket, TYPE>(RULE, &prot::ClientPacket::PTR)
const tg::Pack<prot::ClientPacket> clientPacketPack =
    tg::ClassPack<prot::ClientPacket>(
        member(prot::ClientPacket::Type, type,
               tg::EnumPack<prot::ClientPacket::Type>(3)),
        member(optional<std::string>, stringData, tg::optStringPack),
        member(optional<PlayerDelta>, playerDelta,
               tg::OptionalPack<PlayerDelta>(playerDeltaPack))
    );
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
      "NotifyDelta";
    case Type::NotifyMessage:
      return "NotifyMessage";
  }
}

#define member(TYPE, PTR, RULE) \
  tg::MemberRule<prot::ServerPacket, TYPE>(RULE, &prot::ServerPacket::PTR)
const tg::Pack<prot::ServerPacket> serverPacketPack =
    tg::ClassPack<prot::ServerPacket>(
        member(prot::ServerPacket::Type, type,
               tg::EnumPack<prot::ServerPacket::Type>(3)),
        member(optional<std::string>, stringData, tg::optStringPack),
        member(optional<PID>, pid, tg::OptionalPack<PID>(pidPack)),
        member(optional<ArenaInitializer>, arenaInitializer,
               tg::OptionalPack<ArenaInitializer>(arenaInitializerPack)),
        member(optional<ArenaDelta>, arenaDelta,
               tg::OptionalPack<ArenaDelta>(arenaDeltaPack))
    );
#undef member

}
}