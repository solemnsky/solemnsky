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

const tg::Pack<std::string> stringPack = tg::StringPack();

#define member(TYPE, PTR, RULE) \
  MemberRule<ClientMessage, TYPE>(RULE, &ClientMessage::PTR)
const tg::Pack<ClientMessage> clientMessagePack =
  tg::ClassPack(
    member(ClientMessage::Type, type, tg::EnumPack(1)),
    member(std::string, joke, stringPack)
  );
#undef member

};

}

#endif //SOLEMNSKY_PROTOCOL_H
