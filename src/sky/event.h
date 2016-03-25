/**
 * Things that can happen. Useful for [debug] logging and displays.
 */
#pragma once
#include "util/types.h"
#include "arena.h"
#include "protocol.h"
#include <SFML/Network.hpp>

/**
 * ArenaEvent represents a thing that can happen in an game server. Used by
 * both ClientEvent and ServerEvent.
 */

struct ArenaEvent {
  enum class Type {
    Chat, Broadcast, Join, Quit, NickChange, TeamChange,
    LobbyStart, GameStart, ScoringStart
  } type;

 private:
  ArenaEvent(const Type type);

 public:
  ArenaEvent() = delete;

  optional<std::string> name, addr, message, newName;
  optional<Team> oldTeam, newTeam;

  std::string print() const;

  static ArenaEvent Chat(const std::string &name, const std::string &message);
  static ArenaEvent Broadcast(const std::string &message);

  static ArenaEvent Join(const std::string &name);
  static ArenaEvent Quit(const std::string &name);
  static ArenaEvent NickChange(const std::string &name,
                               const std::string &newName);
  static ArenaEvent TeamChange(const std::string &name, const Team oldTeam,
                               const Team newTeam);
  static ArenaEvent LobbyStart();
  static ArenaEvent GameStart(const std::string &name);
  static ArenaEvent ScoringStart();
};

/**
 * A thing that can happen on a server. Subordinates ArenaEvent.
 */

struct ServerEvent {
  enum class Type {
    Start, Event, Stop
  } type;

 private:
  ServerEvent(const Type type);

 public:
  ServerEvent() = delete;

  optional<unsigned char> port;
  optional<std::string> name;
  optional<ArenaEvent> arenaEvent;
  optional<double> uptime;

  std::string print() const;

  static ServerEvent Start(const unsigned char port,
                           const std::string &name);
  static ServerEvent Event(const ArenaEvent &arenaEvent);
  static ServerEvent Stop(const double uptime);
};

/**
 * A thing that can happen on a client. Subordinates ArenaEvent.
 */

enum class DisconnectType {
  Graceful, Timeout
};

struct ClientEvent {
  enum class Type {
    Connect, Event, Disconnect
  } type;

 private:
  ClientEvent(const Type type);

 public:
  ClientEvent() = delete;

  optional<std::string> name;
  optional<sf::IpAddress> ipAddr;
  optional<ArenaEvent> arenaEvent;
  optional<double> uptime;
  optional<DisconnectType> disconnect;

  std::string print() const;

  static ClientEvent Connect(const std::string &name,
                             const sf::IpAddress &ipAddr);
  static ClientEvent Event(const ArenaEvent &arenaEvent);
  static ClientEvent Disconnect(const double uptime,
                                const DisconnectType disconnect);
};
