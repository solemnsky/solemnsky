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

/**
 * Protocol verbs for the client.
 */
struct ClientPacket {
  enum class Type {
    Ping, // request a pong, always available
    ReqJoin, // request to set your player data (enet is already connected)
    ReqDelta, // request a change to your player data
    Chat // send a chat message
  };

  ClientPacket() = default;

  ClientPacket(const Type type,
               const optional<std::string> &stringData = {},
               const optional<PlayerDelta> &playerDelta = {}) :
      type(type),
      stringData(stringData),
      playerDelta(playerDelta) { }

  Type type;
  optional<std::string> stringData;
  optional<PlayerDelta> playerDelta;

  // debug
  std::string dump() const;
};

extern const tg::Pack<ClientPacket> clientPacketPack;

struct ClientPing: public ClientPacket {
  ClientPing() : ClientPacket(ClientPacket::Type::Ping) { }
};

struct ClientReqJoin: public ClientPacket {
  ClientReqJoin(const std::string &nickname) :
      ClientPacket(ClientPacket::Type::ReqJoin, nickname) { }
};

struct ClientReqDelta: public ClientPacket {
  ClientReqDelta(const PlayerDelta &playerDelta) :
      ClientPacket(ClientPacket::Type::ReqDelta, {}, playerDelta) { }
};

struct ClientChat: public ClientPacket {
  ClientChat(std::string &&str) :
      ClientPacket(ClientPacket::Type::Chat, str) { }
};

/**
 * Protocol verbs for the server.
 */
struct ServerPacket {
  enum class Type {
    Pong,
    AckJoin, // acknowledge a ReqJoin, send ArenaInitializer
    NotifyDelta, // notify clients of a change in the arena
    NotifyMessage // distribute message for all clients to log
  };

  ServerPacket() = default;

  ServerPacket(const Type type,
               const optional<std::string> &stringData = {},
               const optional<PID> &pid = {},
               const optional<ArenaInitializer> &arena = {},
               const optional<ArenaDelta> &arenaDelta = {}) :
      type(type),
      stringData(stringData),
      pid(pid),
      arenaInitializer(arena),
      arenaDelta(arenaDelta) { }

  Type type;
  optional<std::string> stringData;
  optional<PID> pid;
  optional<ArenaInitializer> arenaInitializer;
  optional<ArenaDelta> arenaDelta;

  // debug
  std::string dump() const;
};

extern const tg::Pack<ServerPacket> serverPacketPack;

struct ServerPong: public ServerPacket {
  ServerPong() : ServerPacket(ServerPacket::Type::Pong) { }
};

struct ServerAckJoin: public ServerPacket {
  ServerAckJoin(const PID pid, const ArenaInitializer &arenaInitializer) :
      ServerPacket(ServerPacket::Type::AckJoin, {}, pid, arenaInitializer) { }
};

struct ServerNotifyDelta: public ServerPacket {
  ServerNotifyDelta(const ArenaDelta &arenaDelta) :
      ServerPacket(ServerPacket::Type::NotifyDelta, {}, {}, {}, arenaDelta) { }
};

struct ServerNotifyMessage: public ServerPacket {
  ServerNotifyMessage(const std::string &message,
                      const optional<PID> pid = {}) :
      ServerPacket(ServerPacket::Type::NotifyMessage, message, pid) { }
};

}

#endif //SOLEMNSKY_PROTOCOL_H
