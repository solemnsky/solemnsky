/**
 * Datatypes representing multiplayer protocol messages; also associated Pack<>
 * rules. There are the things that zip across the network.
 */
#ifndef SOLEMNSKY_PROTOCOL_H
#define SOLEMNSKY_PROTOCOL_H

#include "util/types.h"
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
    Chat, // send a chat message
    Snap // transmit a server-bound game snapshot
  };

  ClientPacket() = default;

  ClientPacket(const Type type,
               const optional<std::string> &stringData = {},
               const optional<PlayerDelta> &playerDelta = {},
               const optional<SkySnapshot> &snapshotData = {}) :
      type(type),
      stringData(stringData),
      playerDelta(playerDelta),
      type(snapshotData) { }

  Type type;
  optional<std::string> stringData;
  optional<PlayerDelta> playerDelta;
  optional<SkySnapshot> snapshotData;

  // debug
  std::string dump() const;
};

struct ClientPacketPack: public tg::ClassPack<ClientPacket> {
  ClientPacketPack();
};

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

struct ClientTransmitSnap: public ClientPacket {
  ClientTransmitSnap(const SkySnapshot &snapshot) :
      ClientPacket(ClientPacket::Type::Snap, {}, {}, snapshot) { }
};

/**
 * Protocol verbs for the server.
 */
struct ServerPacket {
  enum class Type {
    Pong,
    AckJoin, // acknowledge a ReqJoin, send ArenaInitializer
    NotifyDelta, // notify clients of a change in the arena
    NotifyMessage, // distribute message for all clients to log
    Snap // transmit a client-bound snapshot
  };

  ServerPacket() = default;

  ServerPacket(const Type type,
               const optional<std::string> &stringData = {},
               const optional<PID> &pid = {},
               const optional<ArenaInitializer> &arena = {},
               const optional<ArenaDelta> &arenaDelta = {},
               const optional<SkySnapshot> snapshotData) :
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
  optional<SkySnapshot> snapshotData;

  // debug
  std::string dump() const;
};

struct ServerPacketPack: public tg::ClassPack<ServerPacket> {
  ServerPacketPack();
};

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

struct ServerSnap: public ServerPacket {
  ServerSnap(const SkySnapshot &snapshot) :
      ServerPacket(ServerPacket::Type::Snap, {}, {}, {}, {}, snapshot) { }
};

}

#endif //SOLEMNSKY_PROTOCOL_H
