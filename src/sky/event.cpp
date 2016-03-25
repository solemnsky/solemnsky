#include "event.h"

/**
 * ArenaEvent.
 */

ArenaEvent::ArenaEvent(const Type type) : type(type) { }

ArenaEvent ArenaEvent::Chat(const std::string &name,
                            const std::string &message) {
  ArenaEvent event(Type::Chat);
  event.name = name;
  event.message = message;
  return event;
}

ArenaEvent ArenaEvent::Broadcast(const std::string &message) {
  ArenaEvent event(Type::Broadcast);
  event.message = message;
  return event;
}

ArenaEvent ArenaEvent::Join(const std::string &name) {
  ArenaEvent event(Type::Join);
  event.name = name;
  return event;
}

ArenaEvent ArenaEvent::Quit(const std::string &name) {
  ArenaEvent event(Type::Quit);
  event.name = name;
  return event;
}

ArenaEvent ArenaEvent::NickChange(const std::string &name,
                                  const std::string &newName) {
  ArenaEvent event(Type::NickChange);
  event.name = name;
  event.newName = newName;
  return event;
}

ArenaEvent ArenaEvent::TeamChange(const std::string &name,
                                  const Team oldTeam,
                                  const Team newTeam) {
  ArenaEvent event(Type::TeamChange);
  event.name = name;
  event.oldTeam = oldTeam;
  event.newTeam = newTeam;
  return event;
}

ArenaEvent ArenaEvent::LobbyStart() {
  return ArenaEvent(Type::LobbyStart);
}

ArenaEvent ArenaEvent::GameStart(const std::string &name) {
  ArenaEvent event(Type::LobbyStart);
  event.name = name;
  return event;
}

ArenaEvent ArenaEvent::ScoringStart() {
  return ArenaEvent(Type::ScoringStart);
}

/**
 * ServerEvent.
 */

std::string ServerEvent::print() const {
  return "<server event>";
}

/**
 * ClientEvent.
 */

ClientEvent::ClientEvent(const Type type) : type(type) { }

std::string ClientEvent::print() const {
  switch (type) {
    case Type::Connect:
      return "Connected to " + inQuotes(*name) + " at " + *addr;
    case Type::Chat:
      return "[chat] " + *name + ": " + *message;
    case Type::Broadcast:
      return "[server] " + *message;
    case Type::Join:
      return inQuotes(*name) + " joined the game!";
    case Type::Quit:
      return inQuotes(*name) + " left the game!";
    case Type::NickChange:
      return inQuotes(*name) + " changed name to " + inQuotes(*newName);
    case Type::TeamChange:
      return inQuotes(*name) + " changed team from " + std::to_string(*oldTeam)
          + " to " + std::to_string(*newTeam);
    case Type::LobbyStart:
      return "** Entered lobby. **";
    case Type::GameStart:
      return "** Started game on " + *name + " **";
    case Type::ScoringStart:
      return "** Entered scoring screen. **";
  }
}

ClientEvent ClientEvent::Connect(const std::string &name,
                                 const std::string &addr) {
  ClientEvent event(Type::Connect);
  event.name = name;
  event.addr = addr;
  return event;
}

