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
#include "protocol.hpp"
#include "util/methods.hpp"

namespace sky {

/**
 * ClientPacket.
 */

ClientPacket::ClientPacket() : ClientPacket(Type()) {}
ClientPacket::ClientPacket(const Type type) : type(type) {}

bool ClientPacket::verifyStructure() const {
  switch (type) {
    case Type::Pong:
      return verifyRequiredOptionals(pingTime, pongTime);
    case Type::ReqJoin:
      return verifyRequiredOptionals(stringData);
    case Type::ReqSky:
      return true;
    case Type::ReqPlayerDelta:
      return verifyRequiredOptionals(playerDelta);
    case Type::ReqInput:
      return verifyRequiredOptionals(participationInput);
    case Type::ReqTeam:
      return verifyRequiredOptionals(team);
    case Type::ReqSpawn:
      return true;
    case Type::Chat:
      return verifyRequiredOptionals(stringData);
    case Type::RCon:
      return verifyRequiredOptionals(stringData);
  }
  return false;
}

ClientPacket ClientPacket::Pong(const Time pingTime,
                                const Time pongTime) {
  ClientPacket packet(ClientPacket::Type::Pong);
  packet.pingTime = pingTime;
  packet.pongTime = pongTime;
  return packet;
}

ClientPacket ClientPacket::ReqJoin(const std::string &nickname) {
  ClientPacket packet(Type::ReqJoin);
  packet.stringData = nickname;
  return packet;
}

ClientPacket ClientPacket::ReqPlayerDelta(const PlayerDelta &playerDelta) {
  ClientPacket packet(Type::ReqPlayerDelta);
  packet.playerDelta = playerDelta;
  return packet;
}

ClientPacket ClientPacket::ReqInput(const ParticipationInput &input) {
  ClientPacket packet(Type::ReqInput);
  packet.participationInput = input;
  return packet;
}

ClientPacket ClientPacket::ReqTeam(const Team team) {
  ClientPacket packet(Type::ReqTeam);
  packet.team = team;
  return packet;
}

ClientPacket ClientPacket::ReqSpawn() {
  return ClientPacket(ClientPacket::Type::ReqSpawn);
}

ClientPacket ClientPacket::Chat(const std::string &message) {
  ClientPacket packet(Type::Chat);
  packet.stringData = message;
  return packet;
}

ClientPacket ClientPacket::RCon(const std::string &command) {
  ClientPacket packet(Type::RCon);
  packet.stringData = command;
  return packet;
}

/**
 * ServerPacket.
 */

ServerPacket::ServerPacket() : ServerPacket(Type()) {}

ServerPacket::ServerPacket(const Type type) : type(type) {}

bool ServerPacket::verifyStructure() const {
  switch (type) {
    case Type::Ping:
      return verifyRequiredOptionals(timestamp);
    case Type::Init:
      return verifyRequiredOptionals(pid, arenaInit, skyHandleInit, scoreInit);
    case Type::InitSky:
      return verifyRequiredOptionals(skyInit);
    case Type::DeltaArena:
      return verifyRequiredOptionals(arenaDelta);
    case Type::DeltaSkyHandle:
      return verifyRequiredOptionals(skyHandleDelta);
    case Type::DeltaSky:
      return verifyRequiredOptionals(timestamp, skyDelta);
    case Type::DeltaScore:
      return verifyRequiredOptionals(scoreDelta);
    case Type::Chat:
      return verifyRequiredOptionals(pid, stringData);
    case Type::Broadcast:
      return verifyRequiredOptionals(stringData);
    case Type::RCon:
      return verifyRequiredOptionals(stringData);
  }
  return false;
}

ServerPacket ServerPacket::Ping(const Time pingTime) {
  ServerPacket packet(ServerPacket::Type::Ping);
  packet.timestamp = pingTime;
  return packet;
}

ServerPacket ServerPacket::Init(const PID pid,
                                const ArenaInit &arenaInit,
                                const SkyHandleInit &skyInit,
                                const ScoreboardInit &scoreInit) {
  ServerPacket packet(Type::Init);
  packet.pid = pid;
  packet.arenaInit = arenaInit;
  packet.skyHandleInit = skyInit;
  packet.scoreInit = scoreInit;
  return packet;
}

ServerPacket ServerPacket::InitSky(const SkyInit &skyInit) {
  ServerPacket packet(Type::InitSky);
  packet.skyInit = skyInit;
  return packet;
}

ServerPacket ServerPacket::DeltaArena(const ArenaDelta &arenaDelta) {
  ServerPacket packet(Type::DeltaArena);
  packet.arenaDelta = arenaDelta;
  return packet;
}

ServerPacket ServerPacket::DeltaSkyHandle(
    const SkyHandleDelta &skyHandleDelta) {
  ServerPacket packet(Type::DeltaSky);
  packet.skyHandleDelta = skyHandleDelta;
  return packet;
}

ServerPacket ServerPacket::DeltaSky(const SkyDelta &skyDelta,
                                    const Time pingTime) {
  ServerPacket packet(Type::DeltaSky);
  packet.skyDelta = skyDelta;
  packet.timestamp = pingTime;
  return packet;
}

ServerPacket ServerPacket::DeltaScore(const ScoreboardDelta &scoreDelta) {
  ServerPacket packet(Type::DeltaScore);
  packet.scoreDelta = scoreDelta;
  return packet;
}

ServerPacket ServerPacket::Chat(const PID pid, const std::string &chat) {
  ServerPacket packet(Type::Chat);
  packet.pid = pid;
  packet.stringData = chat;
  return packet;
}

ServerPacket ServerPacket::Broadcast(const std::string &broadcast) {
  ServerPacket packet(Type::Broadcast);
  packet.stringData = broadcast;
  return packet;
}

ServerPacket ServerPacket::RCon(const std::string &message) {
  ServerPacket packet(Type::RCon);
  packet.stringData = message;
  return packet;
}

}
