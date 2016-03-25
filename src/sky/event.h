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
 * ServerEvent.
 */

struct ServerEvent {
 public:
  enum class Type {
    Start, Connect, Join, Delta, Quit, Message, Stop
  } type;

  // STUB
  ServerEvent() = default;

  std::string print() const;
};

/**
 * ClientEvent.
 */

struct ClientEvent {
  enum class Type {
    Connect,
    Chat,
    Broadcast,
    Join, // resolved subset of ArenaDelta
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

  std::string print() const;

  static ClientEvent Connect(const std::string &name, const std::string &addr);
};
