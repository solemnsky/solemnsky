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
  ClientPacket() = default;

  enum class Type {
    Ping, // request ping
    MotD, // request MotD
    Chat // try to send a chat message
  } type;

  optional<std::string> stringData;
};

struct ClientPing : public ClientPacket {
  ClientPing() : ClientPacket(),
                 type(ClientPacket::Type::Ping) { }
};

struct ClientMotD : public ClientPacket {
  ClientMotD() : ClientPacket(),
                 type(ClientPacket::Type::MotD) { }
};

struct ClientChat : public ClientPacket {
  ClientChat(std::string &&str) : ClientPacket(),
                                  type(ClientPacket::Type::Chat),
                                  stringData(str) { }
};

/**
 * Stuff servers say.
 */
struct ServerPacket {
  ServerPacket() = default;

  enum class Type {
    Pong, // respond to a ping
    MotD, // distribute the MotD
    Message // distribute message
  } type;

  optional<std::string> stringData;
};

struct ServerPong : public ServerPacket {
  ServerPong() : ServerPacket(),
                 type(ServerPacket::Type::Pong) { }
};

struct ServerMotD : public ServerPacket {
  ServerMotD(std::string &&motd) : ServerPacket(),
                                   type(ServerPacket::Type::MotD),
                                   stringData(motd) { }
};

struct ServerMessage : public ServerPacket {
  ServerMessage(std::string &&message) : ServerPacket(),
                                         type(ServerPacket::Type::Message),
                                         stringData(message) { }

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
  MemberRule<ClientPacket, TYPE>(RULE, &ClientPacket::PTR)
const Pack<ClientPacket> clientMessagePack =
    ClassPack<ClientPacket>(
        member(ClientPacket::Type, type, EnumPack<ClientPacket::Type>(1)),
        member(optional<std::string>, stringData, optStringPack)
    );
#undef member

#define member(TYPE, PTR, RULE) \
  MemberRule<ServerPacket, TYPE>(RULE, &ServerPacket::PTR)
const Pack<ServerPacket> serverMessagePack =
    ClassPack<ServerPacket>(
        member(ServerPacket::Type, type, EnumPack<ServerPacket::Type>(1)),
        member(optional<std::string>, stringData, optStringPack)
    );
#undef member
};
}

#endif //SOLEMNSKY_PROTOCOL_H
