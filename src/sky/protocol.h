/**
 * Datatypes representing multiplayer protocol messages; also associated Pack<>
 * rules. There are the things that zip across the network.
 */
#ifndef SOLEMNSKY_PROTOCOL_H
#define SOLEMNSKY_PROTOCOL_H

#include "util/types.h"
#include "telegraph/pack.h"

namespace sky {
/**
 * Stuff clients say.
 */
struct ClientMessage {
  enum class Type {
    Ping,
    MotD // ask for the MotD
  } type;

  std::string joke;
  // all messages to the master server require an associated joke
};

/**
 * Stuff servers say.
 */
struct ServerMessage {
  enum class Type {
    Pong, // respond to a ping
    MotD // respond with the MotD
  } type;

  optional<std::string> motd;
};

/****
 * Packing rules.
 */
namespace pk {
// we have tg in scope in this namespace

const Pack<std::string> stringPack = StringPack();
const Pack<optional<std::string>> optStringPack =
  tg::OptionalPack<std::string>(stringPack);

#define member(TYPE, PTR, RULE) \
  MemberRule<ClientMessage, TYPE>(RULE, &ClientMessage::PTR)
const Pack<ClientMessage> clientMessagePack =
  ClassPack<ClientMessage>(
    member(ClientMessage::Type, type, EnumPack<ClientMessage::Type>(1)),
    member(std::string, joke, stringPack)
  );
#undef member

#define member(TYPE, PTR, RULE) \
  MemberRule<ServerMessage, TYPE>(RULE, &ServerMessage::PTR)
const Pack<ServerMessage> serverMessagePack =
    ClassPack<ServerMessage>(
        member(ServerMessage::Type, type, EnumPack<ServerMessage::Type>(1)),
        member(optional<std::string>, motd, optStringPack)
    );
#undef member
};

}

#endif //SOLEMNSKY_PROTOCOL_H
