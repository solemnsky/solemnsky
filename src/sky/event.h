/**
 * Things that can happen. Useful for [debug] logging and displays.
 */
#pragma once
#include "util/types.h"
#include "arena.h"
#include "protocol.h"
#include <SFML/Network.hpp>

/**
 * ArenaEvent.
 *
 * Aims to be
 */

struct ArenaEvent {

};

/**
 * ServerEvent.
 */

struct ServerEvent {
 public:
  enum class Type {
    Start, Connect, Join, Delta, Quit, Message, Stop
  } type;

 private:
  ServerEvent(const Type type);

 public:
  optional<sf::IpAddress> ipAddr;
  optional<std::pair<unsigned char, sky::ArenaInitializer>> start;
  optional<sky::PlayerInitializer> playerInitializer;
  optional<sky::ArenaDelta> arenaDelta;
  optional<sky::ServerMessage> message;

  std::string printReadable() const;

  static ServerEvent Start(const unsigned char port,
                           const sky::ArenaInitializer &initializer);
  static ServerEvent Connect(const sf::IpAddress &ipAddr);
  static ServerEvent Join(const sf::IpAddress &ipAddr,
                          const sky::PlayerInitializer &initializer);
  static ServerEvent Delta(const sky::ArenaDelta &delta);
  static ServerEvent Message(const sky::ServerMessage &message);
  static ServerEvent Quit(const sf::IpAddress &ipAddr,
                          const sky::PlayerInitializer &initializer);
  static ServerEvent Stop();
};

/**
 * ClientEvent.
 */

struct ClientEvent {
  enum class Type {
    Connect,
    Chat,
    Broadcast,

    // resolved subset of ArenaDelta
        Join,
    Quit,
    NickChange,
    TeamChange,
    LobbyStart,
    GameStart,
    ScoringStart
  } type;

 private:
  ClientEvent(const Type type);

 public:
  ClientEvent() = delete;

  optional<std::string> name, addr, message, newName;
  optional<Team> oldTeam, newTeam;

  std::string print() const;

  static ClientEvent Connect(const std::string &name, const std::string &addr);
  static ClientEvent Chat(const std::string &name, const std::string &message);
  static ClientEvent Broadcast(const std::string &message);

  static ClientEvent Join(const std::string &name);
  static ClientEvent Quit(const std::string &name);
  static ClientEvent NickChange(const std::string &name,
                                const std::string &newName);
  static ClientEvent TeamChange(const std::string &name, const Team oldTeam,
                                const Team newTeam);
  static ClientEvent LobbyStart();
  static ClientEvent GameStart(const std::string &name);
  static ClientEvent ScoringStart();
};
