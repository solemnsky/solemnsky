/**
 * Datatypes representing multiplayer protocol messages; also associated Pack<>
 * rules. There are the things that zip across the network.
 */
#ifndef SOLEMNSKY_PROTOCOL_H
#define SOLEMNSKY_PROTOCOL_H

#include "util/types.h"
#include "delta.h"
#include "telegraph/pack.h"
#include "arena.h"
#include <map>

namespace sky {
namespace prot {

/**
 * Stuff clients say.
 */
struct ClientPacket {
  enum class Type {
    Ping, // request a pong, always available
    ReqConnection, // request to finalize connection (enet is already connected)

    // available only when joined in arena
        Chat // try to send a chat message
  };

  ClientPacket() = default;

  ClientPacket(const Type type,
               const optional<std::string> &stringData = {}) :
      type(type),
      stringData(stringData) { }

  Type type;
  optional<std::string> stringData;
};

struct ClientPing: public ClientPacket {
  ClientPing() : ClientPacket(ClientPacket::Type::Ping) { }
};

struct ClientReqConnection: public ClientPacket {
  ClientReqConnection(const std::string &nick) :
      ClientPacket(ClientPacket::Type::ReqConnection, nick) { }
};

struct ClientChat: public ClientPacket {
  ClientChat(std::string &&str) :
      ClientPacket(ClientPacket::Type::Chat, str) { }
};

/**
 * Stuff servers say.
 */
struct ServerPacket {
  enum class Type {
    Pong,
    AcceptConnection, // accept a connection
    NotifyConnection, // notify clients of a new player record
    NotifyRecordDelta, // notify clients of a record delta
    NotifyDisconnection, // notify clients of a player record removal
    NotifyMessage // distribute message for all clients to log
  };

  ServerPacket() = default;

  ServerPacket(const Type type,
               const optional<std::string> &stringData = {},
               const optional<PID> &pid = {},
               const optional<Arena> &arena = {},
               const optional<PlayerRecord> &record = {},
               const optional<PlayerRecordDelta> &recordDelta = {}) :
      type(type),
      stringData(stringData),
      pid(pid),
      arena(arena),
      record(record),
      recordDelta(recordDelta) { }

  Type type;
  optional<std::string> stringData;
  optional<PID> pid;
  optional<Arena> arena;
  optional<PlayerRecord> record;
  optional<PlayerRecordDelta> recordDelta;
};

struct ServerPong: public ServerPacket {
  ServerPong() : ServerPacket(ServerPacket::Type::Pong) { }
};

struct ServerAcceptConnection: public ServerPacket {
  ServerAcceptConnection(const PID pid,
                         const Arena &arena) :
      ServerPacket(ServerPacket::Type::AcceptConnection, {}, pid, arena) { }
};

struct ServerNotifyConnection: public ServerPacket {
  ServerNotifyConnection(const PlayerRecord &record) :
      ServerPacket(ServerPacket::Type::NotifyConnection, {}, {}, {}, record) { }
};

struct ServerNotifyRecordDelta: public ServerPacket {
  ServerNotifyRecordDelta(const PID pid,
                          const PlayerRecordDelta &recordDelta) :
      ServerPacket(ServerPacket::Type::NotifyRecordDelta, {},
                   pid, {}, {}, recordDelta) { }
};

struct ServerNotifyDisconnection: public ServerPacket {
  ServerNotifyDisconnection(const std::string &reason, const PID pid) :
      ServerPacket(ServerPacket::Type::NotifyDisconnection, reason, pid) { }
};

struct ServerNotifyMessage: public ServerPacket {
  // no PID supplied if it's not from a client
  ServerNotifyMessage(const std::string &message,
                      const optional<PID> pid = {}) :
      ServerPacket(ServerPacket::Type::NotifyMessage, message, pid) { }
};
}

/****
 * Packing rules.
 */
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
               EnumPack<prot::ClientPacket::Type>(2)),
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
               EnumPack<prot::ServerPacket::Type>(2)),
        member(optional<std::string>, stringData, optStringPack),
        member(optional<Arena>, arena, OptionalPack<Arena>(arenaPack))
    );
#undef member

}

}

#endif //SOLEMNSKY_PROTOCOL_H
