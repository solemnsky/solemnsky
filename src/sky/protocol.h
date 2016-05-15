/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * Protocol verbs constituting the network protocol.
 */
#pragma once
#include "util/types.h"
#include "participation.h"
#include "sky.h"
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
    ReqAction, // request to perform an action

    ReqSpawn, // request to spawn

    Chat, // send a chat message
    RCon  // send an rcon command
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
      case Type::ReqAction: {
        ar(action, state);
        break;
      }
      case Type::ReqSpawn: {
        break;
      }
      case Type::Chat: {
        ar(stringData);
      }
      case Type::RCon: {
        ar(stringData);
      }
    }
  }

  ClientPacket();
  ClientPacket(const Type type);

  Type type;
  optional<std::string> stringData;
  optional<PlayerDelta> playerDelta;
  optional<Action> action;
  optional<bool> state;

  bool verifyStructure() const override;

  static ClientPacket Ping();
  static ClientPacket ReqJoin(const std::string &nickname);
  static ClientPacket ReqPlayerDelta(const PlayerDelta &playerDelta);
  static ClientPacket ReqAction(const Action &action, const bool state);
  static ClientPacket ReqSpawn();
  static ClientPacket Chat(const std::string &message);
  static ClientPacket RCon(const std::string &command);
};

/**
 * Protocol verbs for the server.
 */
struct ServerPacket: public VerifyStructure {
  enum class Type {
    Pong,
    Init, // acknowledge a ReqJoin, send ArenaInit
    DeltaArena, // broadcast a change in the Arena
    DeltaSky, // broadcast a change in the Arena's sky

    Chat, // chat relay, to all clients
    Broadcast, // broadcast message, to any subset of clients
    RCon // rcon message, to one client
  };

  ServerPacket();
  ServerPacket(const Type type);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Pong:
        break;
      case Type::Init: {
        ar(pid, arenaInit, skyInit);
        break;
      }
      case Type::DeltaArena: {
        ar(arenaDelta);
        break;
      }
      case Type::DeltaSky: {
        ar(skyDelta);
        break;
      }
      case Type::Chat: {
        ar(pid, stringData);
        break;
      }
      case Type::Broadcast: {
        ar(stringData);
        break;
      }
      case Type::RCon: {
        ar(stringData);
        break;
      }
    }
  }

  Type type;
  optional<PID> pid;
  optional<ArenaInit> arenaInit;
  optional<ArenaDelta> arenaDelta;
  optional<SkyInitializer> skyInit;
  optional<SkyDelta> skyDelta;
  optional<std::string> stringData;

  bool verifyStructure() const override;

  static ServerPacket Pong();
  static ServerPacket Init(const PID pid, const ArenaInit &arenaInit,
                           const SkyInitializer &init);
  static ServerPacket DeltaArena(const ArenaDelta &arenaDelta);
  static ServerPacket DeltaSky(const SkyDelta &skyDelta);
  static ServerPacket Chat(const PID pid, const std::string &chat);
  static ServerPacket Broadcast(const std::string &broadcast);
  static ServerPacket RCon(const std::string &message);
};

}
