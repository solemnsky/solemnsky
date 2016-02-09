/**
 * Datatypes representing multiplayer protocol messages; also associated Pack<>
 * rules. There are the things that zip across the network.
 */
#ifndef SOLEMNSKY_PROTOCOL_H
#define SOLEMNSKY_PROTOCOL_H

#include "util/types.h"
#include "telegraph/pack.h"
#include "arena.h"

namespace sky {
namespace prot {
/**
 * Stuff clients say.
 */
struct ClientPacket {
  enum class Type {
    Ping,
    MotD, // request MotD

    ReqConnection, // request a connection, supplying a preferred nickname
    ReqNickChange, // request to change the nickname, supplying it

    Chat // try to send a chat message
  };

  ClientPacket() = default;

  ClientPacket(const Type type,
               const optional<std::string> &stringData = {}) :
      type(type), stringData(stringData) { }

  Type type;
  optional<std::string> stringData;
};

struct ClientPing : public ClientPacket {
  ClientPing() : ClientPacket(ClientPacket::Type::Ping) { }
};

struct ClientReqConnection : public ClientPacket {
  ClientReqConnection(std::string &&nick) :
      ClientPacket(ClientPacket::Type::ReqConnection, nick) { }
};

struct ClientReqNickChange : public ClientPacket {
  ClientReqNickChange(std::string &&nick) :
      ClientPacket(ClientPacket::Type::ReqNickChange, nick) { }
};


struct ClientMotD : public ClientPacket {
  ClientMotD() : ClientPacket(ClientPacket::Type::MotD) { }
};

struct ClientChat : public ClientPacket {
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
    MotD, // distribute the MotD
    Message // distribute message for all clients to print to screen
  };

  ServerPacket() = default;

  ServerPacket(const Type type,
               const optional<std::string> &stringData = {}) :
      type(type), stringData(stringData) { }

  Type type;
  optional<std::string> stringData;

  optional<Arena> arena; // for AcceptConnection
  optional<PID> pid; // for AcceptConnection
};

struct ServerPong : public ServerPacket {
  ServerPong() : ServerPacket(ServerPacket::Type::Pong) { }
};

struct ServerAssignNick : public ServerPacket {
  ServerAssignNick(std::string &&nick) :
      ServerPacket(ServerPacket::Type::AssignNick, nick) { }
};

struct ServerAcceptConnection : public ServerPacket {
  ServerAcceptConnection()
};

struct ServerMotD : public ServerPacket {
  ServerMotD(std::string &&motd) :
      ServerPacket(ServerPacket::Type::MotD, motd) { }
};

struct ServerMessage : public ServerPacket {
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
    tg::OptionalPack<std::string>(stringPack);

#define member(TYPE, PTR, RULE) \
  MemberRule<prot::ClientPacket, TYPE>(RULE, &prot::ClientPacket::PTR)
const Pack<prot::ClientPacket> clientPacketPack =
    ClassPack<prot::ClientPacket>(
        member(prot::ClientPacket::Type, type,
               EnumPack<prot::ClientPacket::Type>(2)),
        member(optional<std::string>, stringData, optStringPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  MemberRule<prot::ServerPacket, TYPE>(RULE, &prot::ServerPacket::PTR)
const Pack<prot::ServerPacket> serverPacketPack =
    ClassPack<prot::ServerPacket>(
        member(prot::ServerPacket::Type, type,
               EnumPack<prot::ServerPacket::Type>(2)),
        member(optional<std::string>, stringData, optStringPack)
    );
#undef member

}

}

#endif //SOLEMNSKY_PROTOCOL_H
