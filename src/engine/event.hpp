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
 * Things that can happen. Useful for [debug] logging and displays.
 */
#pragma once
#include <SFML/Network.hpp>
#include "util/types.hpp"
#include "util/printer.hpp"
#include "arena.hpp"
#include "protocol.hpp"

namespace sky {

/**
 * onEvent(const ArenaEvent &) is a subsystem callback denoting some event in
 * the Arena. What seperates an ArenaEvent from another subsystem callback is
 * that is is expected to be logged and should therefore be storable
 * in a unified datatype and printable through Printer.
 */

enum class DisconnectType {
  Graceful, Timeout
};

struct ArenaEvent {
  enum class Type {
    Join, Quit, NickChange, TeamChange, ModeChange, EnvChoose
  } type;

 private:
  ArenaEvent(const Type type);

 public:
  ArenaEvent() = delete;

  optional<std::string> name, newName;
  optional<sky::Team> oldTeam, newTeam;
  optional<sky::ArenaMode> mode;
  optional<EnvironmentURL> environment;

  void print(Printer &p) const;

  static ArenaEvent Join(const std::string &name);
  static ArenaEvent Quit(const std::string &name);
  static ArenaEvent NickChange(const std::string &name,
                               const std::string &newName);
  static ArenaEvent TeamChange(const std::string &name, const sky::Team oldTeam,
                               const sky::Team newTeam);
  static ArenaEvent ModeChange(const sky::ArenaMode mode);
  static ArenaEvent EnvChoose(const EnvironmentURL &map);

};

}

/**
 * ArenaEvent + server-specific events.
 */

struct ServerEvent {
  // TODO: engine::DisconnectType in ServerEvent::Disconnect

  enum class Type {
    Start, Event, Stop, Connect, Disconnect, RConIn, RConOut
  } type;

 private:
  ServerEvent(const Type type);

 public:
  ServerEvent() = delete;

  optional<Port> port;
  optional<std::string> stringData;
  optional<sky::ArenaEvent> arenaEvent;
  optional<double> uptime;

  void print(Printer &p) const;

  static ServerEvent Start(const Port port,
                           const std::string &name);
  static ServerEvent Event(const sky::ArenaEvent &arenaEvent);
  static ServerEvent Stop(const double uptime);
  static ServerEvent Connect(const std::string &name);
  static ServerEvent Disconnect(const std::string &name);

  static ServerEvent RConIn(const std::string &command);
  static ServerEvent RConOut(const std::string &response);
};

/**
 * ArenaEvent + client-specific events.
 */

struct ClientEvent {
  enum class Type {
    Connect, Event, Disconnect, Chat, Broadcast,
    RConCommand, RConResponse
  } type;

 private:
  ClientEvent(const Type type);

 public:
  ClientEvent() = delete;

  optional<std::string> name, message;
  optional<sf::IpAddress> ipAddr;
  optional<sky::ArenaEvent> arenaEvent;
  optional<double> uptime;
  optional<sky::DisconnectType> disconnect;

  void print(Printer &p) const;

  static ClientEvent Connect(const std::string &name,
                             const sf::IpAddress &ipAddr);
  static ClientEvent Event(const sky::ArenaEvent &arenaEvent);
  static ClientEvent Disconnect(const double uptime,
                                const sky::DisconnectType disconnect);
  static ClientEvent Chat(const std::string &name,
                          const std::string &message);
  static ClientEvent Broadcast(const std::string &message);

  static ClientEvent RConCommand(const std::string &command);
  static ClientEvent RConResponse(const std::string &response);
};

