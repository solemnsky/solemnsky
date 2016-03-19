/**
 * Protocol verbs constituting the network protocol.
 */
#pragma once
#include "util/types.h"
#include "arena.h"
#include <map>

namespace sky {

/**
 * Protocol verbs for the client.
 */
struct ClientPacket: public VerifyStructure {
  enum class Type {
    Ping, // request a pong, always available
    ReqJoin, // request joining in the arena, part of the connection protocol

    ReqPlayerDelta, // request a change to your player data
    ReqSkyDelta, // request to modify the Sky

    ReqSpawn, // request to spawn
    ReqKill, // request to die

    Chat // send a chat message
  };

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Ping: {
        break;
      }
      case Type::ReqJoin: {
        ar(stringData);
        break;
      }
      case Type::ReqPlayerDelta: {
        ar(playerDelta);
        break;
      }
      case Type::ReqSkyDelta: {
        ar(skyDelta);
        break;
      }
      case Type::ReqSpawn: {
        break;
      }
      case Type::ReqKill: {
        break;
      }
      case Type::Chat: {
        ar(stringData);
      }
    }
  }

  ClientPacket();
  ClientPacket(
      const Type type,
      const optional<std::string> &stringData = {},
      const optional<PlayerDelta> &playerDelta = {},
      const optional<SkyDelta> &skyDelta = {});

  Type type;
  optional<std::string> stringData;
  optional<PlayerDelta> playerDelta;
  optional<SkyDelta> skyDelta;

  bool verifyStructure() const override;

  static ClientPacket Ping();
  static ClientPacket ReqJoin(const std::string &nickname);
  static ClientPacket ReqPlayerDelta(const PlayerDelta &playerDelta);
  static ClientPacket ReqSkyDelta(const SkyDelta &skyDelta);
  static ClientPacket ReqSpawn();
  static ClientPacket ReqKill();
  static ClientPacket Chat(const std::string &message);
};

/**
 * Messages from the server, of varied nature.
 */
struct ServerMessage: public VerifyStructure {
  enum class Type {
    Chat, // a client said something
    Broadcast // an important message to be displayed in big letters
  };

  ServerMessage();
  ServerMessage(
      const Type type,
      std::string contents,
      optional<PID> from = {});

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Chat: {
        ar(from, contents);
        break;
      }
      case Type::Broadcast: {
        ar(contents);
        break;
      }
    }
  }

  Type type;
  optional<PID> from; // who the message is from, potentially
  std::string contents;

  bool verifyStructure() const override;

  static ServerMessage Chat(const PID &from,
                            const std::string &contents);
  static ServerMessage Broadcast(const std::string &contents);
};

/**
 * Protocol verbs for the server.
 */
struct ServerPacket: public VerifyStructure {
  enum class Type {
    Pong,
    Message, // message for a client to log
    Init, // acknowledge a ReqJoin, send ArenaInitializer
    Delta, // notify clients of a change in the arena
  };

  ServerPacket();
  ServerPacket(const ServerPacket::Type type,
               const optional<ServerMessage> &message = {},
               const optional<PID> &pid = {},
               const optional<ArenaInitializer> &init = {},
               const optional<ArenaDelta> &delta = {});

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Pong:
        break;
      case Type::Message: {
        ar(message);
        break;
      }
      case Type::Init: {
        ar(pid, init);
        break;
      }
      case Type::Delta: {
        ar(delta);
        break;
      }
    }
  }

  Type type;
  optional<ServerMessage> message;
  optional<PID> pid;
  optional<ArenaInitializer> init;
  optional<ArenaDelta> delta;

  bool verifyStructure() const override;

  static ServerPacket Pong();
  static ServerPacket Message(const ServerMessage &message);
  static ServerPacket Init(const PID pid, const ArenaInitializer &init);
  static ServerPacket Delta(const ArenaDelta &delta);
};

}
