#include "event.h"
#include "util/methods.h"

namespace sky {

/**
 * ServerEvent.
 */

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

ClientEvent ClientEvent::Chat(const std::string &name,
                              const std::string &message) {
  ClientEvent event(Type::Chat);
  event.name = name;
  event.message = message;
  return event;
}

ClientEvent ClientEvent::Broadcast(const std::string &message) {
  ClientEvent event(Type::Broadcast);
  event.message = message;
  return event;
}

ClientEvent ClientEvent::Join(const std::string &name) {
  ClientEvent event(Type::Join);
  event.name = name;
  return event;
}

ClientEvent ClientEvent::Quit(const std::string &name) {
  ClientEvent event(Type::Quit);
  event.name = name;
  return event;
}

ClientEvent ClientEvent::NickChange(const std::string &name,
                                    const std::string &newName) {
  ClientEvent event(Type::NickChange);
  event.name = name;
  event.newName = newName;
  return event;
}

ClientEvent ClientEvent::TeamChange(const std::string &name,
                                    const Team oldTeam,
                                    const Team newTeam) {
  ClientEvent event(Type::TeamChange);
  event.oldTeam = oldTeam;
  event.newTeam = newTeam;
  return event;
}

ClientEvent ClientEvent::LobbyStart() {
  return ClientEvent(Type::LobbyStart);
}

ClientEvent ClientEvent::GameStart(const std::string &name) {
  ClientEvent event(Type::LobbyStart);
  event.name = name;
  return event;
}

ClientEvent ClientEvent::ScoringStart() {
  return ClientEvent(Type::ScoringStart);
}

}


