/**
 * Datatypes representing multiplayer protocol messages; also associated Pack<>
 * rules. There are the things that zip across the network.
 */
#ifndef SOLEMNSKY_PROTOCOL_H
#define SOLEMNSKY_PROTOCOL_H

#include "util/types.h"
#include "telegraph/pack.h"

namespace sky {
namespace prot {
/**
 * Stuff clients say.
 */
struct ClientPacket {
  enum class Type {
    Ping, // request ping
    MotD, // request MotD
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
    Pong, // respond to a ping
    MotD, // distribute the MotD
    Message // distribute message for all clients to print to screen
  };

  ServerPacket() = default;

  ServerPacket(const Type type,
               const optional<std::string> &stringData = {}) :
      type(type), stringData(stringData) { }

  Type type;
  optional<std::string> stringData;
};

struct ServerPong : public ServerPacket {
  ServerPong() : ServerPacket(ServerPacket::Type::Pong) { }
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
