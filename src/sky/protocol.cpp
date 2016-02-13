#include "protocol.h"

namespace sky {

std::string prot::ClientPacket::dump() const {
  switch (type) {
    case Type::Ping:
      return "Ping";
    case Type::ReqConnection:
      return "ReqConnection: " + *stringData;
    case Type::Chat:
      return "Chat: " + *stringData;
  }
}

std::string prot::ServerPacket::dump() const {
  switch (type) {
    case Type::Pong:
      return "Pong";
    case Type::AcceptConnection:
      return "AcceptConnection";
//      return "AcceptConnection: "; + *pid;
    case Type::NotifyConnection:
      return "NotifyConnection";
//      return "NotifyConnection: " + std::to_string(*pid);
    case Type::NotifyRecordDelta: {
      return "NotifyRecordDelta";
//      if (recordDelta->nickname)
//        return "NotifyRecordDelta: "
//            + std::to_string(*pid) + ", " + *recordDelta->nickname;
//      else return "NotifyRecordDelta: " + std::to_string(*pid);
    }
    case Type::NotifyDisconnection:
      return "NotifyDisconnection";
//      return "NotifyDisconnection: " + std::to_string(*pid)
//          + " (" + *stringData + ")";
    case Type::NotifyMessage:
      return "NotifyMessage: " + *stringData;
  }
}

namespace pk {

using namespace tg;

const Pack<std::string> stringPack = StringPack();
const Pack<optional<std::string>> optStringPack =
    OptionalPack<std::string>(stringPack);
const Pack<PID> pidPack = BytePack<PID>();

/**
 * ClientPacket.
 */
#define member(TYPE, PTR, RULE) \
  MemberRule<prot::ClientPacket, TYPE>(RULE, &prot::ClientPacket::PTR)
const Pack<prot::ClientPacket> clientPacketPack =
    ClassPack<prot::ClientPacket>(
        member(prot::ClientPacket::Type, type,
               EnumPack<prot::ClientPacket::Type>(3)),
        member(optional<std::string>, stringData, optStringPack)
    );
#undef member

/**
 * PlayerRecord.
 */
#define member(TYPE, PTR, RULE) \
  MemberRule<PlayerRecord, TYPE>(RULE, &PlayerRecord::PTR)
const Pack<PlayerRecord> playerRecordPack =
    ClassPack<PlayerRecord>(
        member(bool, connected, boolPack),
        member(std::string, nickname, stringPack)
    );
#undef member

/**
 * PlayerRecordDelta.
 */
#define member(TYPE, PTR, RULE) \
  MemberRule<PlayerRecordDelta, TYPE>(RULE, &PlayerRecordDelta::PTR)
const Pack<PlayerRecordDelta> playerRecordDeltaPack =
    ClassPack<PlayerRecordDelta>(
        member(bool, connected, boolPack),
        member(optional<std::string>, nickname, optStringPack)
    );
#undef member

/**
 * Arena.
 */
#define member(TYPE, PTR, RULE) \
  MemberRule<Arena, TYPE>(RULE, &Arena::PTR)
const Pack<Arena> arenaPack =
    ClassPack<Arena>(
        MemberRule<Arena, std::list<PlayerRecord>>(
            ListPack<PlayerRecord>(playerRecordPack),
            &Arena::playerRecords),
        member(std::string, motd, stringPack)
    );
#undef member

/**
 * ServerPacket.
 */
#define member(TYPE, PTR, RULE) \
  MemberRule<prot::ServerPacket, TYPE>(RULE, &prot::ServerPacket::PTR)
const Pack<prot::ServerPacket> serverPacketPack =
    ClassPack<prot::ServerPacket>(
        member(prot::ServerPacket::Type, type,
               EnumPack<prot::ServerPacket::Type>(3)),
        member(optional<std::string>, stringData, optStringPack),
        member(optional<PID>, pid, OptionalPack<PID>(pidPack)),
        member(optional<Arena>, arena, OptionalPack<Arena>(arenaPack)),
        member(optional<PlayerRecord>, record,
               OptionalPack<PlayerRecord>(playerRecordPack)),
        member(optional<PlayerRecordDelta>, recordDelta,
               OptionalPack<PlayerRecordDelta>(playerRecordDeltaPack))

    );
#undef member

}
}