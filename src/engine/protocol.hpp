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
#include <map>
#include "util/types.hpp"
#include "scoreboard.hpp"
#include "sky/skyhandle.hpp"
#include "arena.hpp"

namespace sky {

/**
 * Protocol verbs for the client.
 */
struct ClientPacket : public VerifyStructure {
  enum class Type {
    Pong, // respond to a server Ping
    ReqJoin, // request joining in the arena, part of the connection protocol
    ReqSky, // request a Sky initializer, having loaded the Environment

    ReqPlayerDelta, // request a change to your player data
    ReqInput, // request an input into your engine Participation

    ReqTeam, // request a team change
    ReqSpawn, // request to spawn

    Chat, // send a chat message
    RCon  // send an rcon command
  };

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Pong: {
        ar(pingTime, pongTime);
        break;
      }
      case Type::ReqJoin: {
        ar(stringData);
        break;
      }
      case Type::ReqSky: {
        break;
      }
      case Type::ReqPlayerDelta: {
        ar(playerDelta);
        break;
      }
      case Type::ReqInput: {
        ar(participationInput);
        break;
      };
      case Type::ReqTeam: {
        ar(team);
        break;
      }
      case Type::ReqSpawn: {
        break;
      }
      case Type::Chat: {
        ar(stringData);
        break;
      }
      case Type::RCon: {
        ar(stringData);
        break;
      }
    }
  }

  ClientPacket();
  ClientPacket(const Type type);

  Type type;
  optional<Time> pingTime, pongTime;
  optional<std::string> stringData;
  optional<PlayerDelta> playerDelta;
  optional<Team> team;
  optional<ParticipationInput> participationInput;
  optional<bool> state;

  bool verifyStructure() const override;

  static ClientPacket Pong(const Time pingTime, const Time pongTime);
  static ClientPacket ReqJoin(const std::string &nickname);
  static ClientPacket ReqPlayerDelta(const PlayerDelta &playerDelta);
  static ClientPacket ReqInput(const ParticipationInput &input);
  static ClientPacket ReqTeam(const Team team);
  static ClientPacket ReqSpawn();
  static ClientPacket Chat(const std::string &message);
  static ClientPacket RCon(const std::string &command);
};

/**
 * Protocol verbs for the server.
 */
struct ServerPacket : public VerifyStructure {
  enum class Type {
    Ping, // request a client Pong

    Init, // acknowledge a ReqJoin, send ArenaInit
    InitSky, // sky initialization for a client who's loaded the environment

    DeltaArena, // broadcast a change in the Arena
    DeltaSkyHandle, // broadcast a change in the SkyHandle
    DeltaSky, // broadcast a change in the Sky
    DeltaScore, // broadcast a change in the Scoreboard

    Chat, // chat relay to all clients
    Broadcast, // broadcast message, to any subset of clients
    RCon // rcon message, to one client
  };

  ServerPacket();
  ServerPacket(const Type type);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Ping: {
        ar(timestamp);
        break;
      }
      case Type::Init: {
        ar(pid, arenaInit, skyHandleInit, scoreInit);
        break;
      }
      case Type::InitSky: {
        ar(skyInit);
        break;
      }
      case Type::DeltaArena: {
        ar(arenaDelta);
        break;
      }
      case Type::DeltaSkyHandle : {
        ar(skyHandleDelta);
        break;
      }
      case Type::DeltaSky: {
        ar(timestamp, skyDelta);
        break;
      }
      case Type::DeltaScore: {
        ar(scoreDelta);
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

  optional<PID> pid;                       // Init
  optional<ArenaInit> arenaInit;
  optional<SkyHandleInit> skyHandleInit;
  optional<ScoreboardInit> scoreInit;
  optional<SkyInit> skyInit;               // InitSky
  optional<ArenaDelta> arenaDelta;         // DeltaArena
  optional<SkyHandleDelta> skyHandleDelta; // DeltaSkyHandle
  optional<SkyDelta> skyDelta;             // DeltaSky
  optional<Time> timestamp;
  optional<ScoreboardDelta> scoreDelta;    // DeltaScore
  optional<std::string> stringData; // Chat, Broadcast, RCon

  bool verifyStructure() const override;

  static ServerPacket Ping(const Time pingTime);
  static ServerPacket Init(const PID pid,
                           const ArenaInit &arenaInit,
                           const SkyHandleInit &skyInit,
                           const ScoreboardInit &scoreInit);
  static ServerPacket InitSky(const SkyInit &skyInit);
  static ServerPacket DeltaArena(const ArenaDelta &arenaDelta);
  static ServerPacket DeltaSkyHandle(const SkyHandleDelta &skyhandleDelta);
  static ServerPacket DeltaSky(const SkyDelta &skyDelta,
                               const Time pingTime);
  static ServerPacket DeltaScore(const ScoreboardDelta &scoreDelta);
  static ServerPacket Chat(const PID pid, const std::string &chat);
  static ServerPacket Broadcast(const std::string &broadcast);
  static ServerPacket RCon(const std::string &message);

};

}
