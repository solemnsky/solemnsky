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
  tg::MemberRule<ClientPacket, TYPE>(RULE, &ClientPacket::PTR)
const tg::Pack<ClientPacket> clientPacketPack =
    tg::ClassPack<ClientPacket>(
        member(ClientPacket::Type, type,
               tg::EnumPack<ClientPacket::Type>(3)),
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
  tg::MemberRule<ServerPacket, TYPE>(RULE, &ServerPacket::PTR)
const tg::Pack<ServerPacket> serverPacketPack =
    tg::ClassPack<ServerPacket>(
        member(ServerPacket::Type, type,
               tg::EnumPack<ServerPacket::Type>(3)),
        member(optional<std::string>, stringData, tg::optStringPack),
        member(optional<PID>, pid, tg::OptionalPack<PID>(pidPack)),
        member(optional<ArenaInitializer>, arenaInitializer,
               tg::OptionalPack<ArenaInitializer>(
                   // arenaInitializerPack // this appears to be null..?
                   // (the rule's functions are not initialized)
                   // it's defined in arena.cpp, as the following:
                   tg::ClassPack<ArenaInitializer>(
                       tg::MemberRule<ArenaInitializer, std::vector<Player>>(
                           tg::VectorPack<Player>(
                               playerPack // and now this is null!!
                               // (also defined in arena.cpp)
                               // I don't want to inline everything...
                           ),
                           &ArenaInitializer::playerRecords),
                       tg::MemberRule<ArenaInitializer, std::string>(
                           tg::stringPack, &ArenaInitializer::motd
                       )
                   )
               )),
        member(optional<ArenaDelta>, arenaDelta,
               tg::OptionalPack<ArenaDelta>(arenaDeltaPack))
    );
#undef member

}

}