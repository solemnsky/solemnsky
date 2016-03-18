/**
 * Things that happen; implied by various systems, from the Arena to the Sky.
 */
#pragma once
#include "arena.h"

namespace sky {

/**
 * An event useful on the server-side, useful for debug logs.
 */
struct ServerEvent {
  // stub
};

/**
 * An event useful on the client-side, useful for on-screen logs.
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

}
