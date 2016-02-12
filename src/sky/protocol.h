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
    Ping,
    ReqConnection, // request to finalize connection (enet is already connected)
    ReqNick, // request to change the nickname, supplying it
    Chat // try to send a chat message
  };

  ClientPacket() = default;

  ClientPacket(const Type type,
               const optional<std::string> &stringData = {}) :
      type(type), stringData(stringData) { }

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

struct ClientReqNick: public ClientPacket {
  ClientReqNick(const std::string &nick) :
      ClientPacket(ClientPacket::Type::ReqNick, nick) { }
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
    AssignNick, // give a client a nickname
    AcceptConnection, // accept a connection
    Message // distribute message for all clients to print to screen
  };

  ServerPacket() = default;

  ServerPacket(const Type type,
               const optional<std::string> &stringData = {},
               const optional<Arena> &arena = {},
               const optional<PID> &pid = {}) :
      type(type), stringData(stringData) { }

  Type type;

  optional<std::string> stringData;
  optional<Arena> arena;
  optional<PID> pid;
};

struct ServerPong: public ServerPacket {
  ServerPong() : ServerPacket(ServerPacket::Type::Pong) { }
};

struct ServerAssignNick: public ServerPacket {
  ServerAssignNick(const PID pid, std::string &&nick) :
      ServerPacket(ServerPacket::Type::AssignNick, nick, {}, pid) { }
};

struct ServerAcceptConnection: public ServerPacket {
  ServerAcceptConnection(const Arena &arena, const PID pid) :
      ServerPacket(ServerPacket::Type::AcceptConnection, {}, arena, pid) { }
};

struct ServerNotifyConnection: public ServerPacket {

};

struct ServerMessage: public ServerPacket {
  ServerMessage(std::string &&message) :
      ServerPacket(ServerPacket::Type::Message, message) { }
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
