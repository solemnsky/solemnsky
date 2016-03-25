#include "event.h"

/**
 * ArenaEvent.
 */

ArenaEvent::ArenaEvent(const Type type) : type(type) { }

std::string ArenaEvent::print() const {
  switch (type) {
    case Type::Chat:
      return "[chat] " + *name + ": " + *message;
    case Type::Broadcast:
      return "[server] " + *message;
    case Type::Join:
      return inQuotes(name.get()) + " joined the game!";
    case Type::Quit:
      return inQuotes(name.get()) + " left the game!";
    case Type::NickChange:
      return inQuotes(name.get()) + " changed name to "
          + inQuotes(newName.get());
    case Type::TeamChange:
      return inQuotes(name.get()) + " changed team from "
          + std::to_string(oldTeam.get())
          + " to " + std::to_string(newTeam.get());
    case Type::LobbyStart:
      return "** Entered lobby. **";
    case Type::GameStart:
      return "** Began game on " + *name + " **";
    case Type::ScoringStart:
      return "** Entered scoring. **";
  }
}

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

ServerEvent::ServerEvent(const Type type) : type(type) { }

std::string ServerEvent::print() const {
  switch (type) {
    case Type::Start:
      return "Started " + inQuotes(name.get())
          + " on port " + std::to_string(port.get());
    case Type::Event:
      return arenaEvent->print();
    case Type::Stop:
      return "Stopped server after " + std::to_string(uptime.get())
          + " seconds of uptime";
  }
}

ServerEvent ServerEvent::Start(const unsigned char port,
                               const std::string &name) {
  ServerEvent event(Type::Start);
  event.port = port;
  event.name = name;
  return event;
}

ServerEvent ServerEvent::Event(const ArenaEvent &arenaEvent) {
  ServerEvent event(Type::Event);
  event.arenaEvent = arenaEvent;
  return event;
}

ServerEvent ServerEvent::Stop(const double uptime) {
  ServerEvent event(Type::Stop);
  event.uptime = uptime;
  return event;
}

/**
 * ClientEvent.
 */

ClientEvent::ClientEvent(const Type type) : type(type) { }


std::string ClientEvent::print() const {
  switch (type) {
    case Type::Connect:
      return "Connected to " + inQuotes(name.get())
          + " @ " + ipAddr->toString();
    case Type::Event:
      return arenaEvent->print();
    case Type::Disconnect: {
      switch (disconnect.get()) {
        case DisconnectType::Graceful:
          return "Disconnected from server gracefully.";
        case DisconnectType::Timeout:
          return "Disconnected from server due to timeout.";
      }
    }
  }
}

ClientEvent ClientEvent::Connect(const std::string &name,
                                 const sf::IpAddress &ipAddr) {
  ClientEvent event(Type::Connect);
  event.name = name;
  event.ipAddr = ipAddr;
  return event;
}

ClientEvent ClientEvent::Event(const ArenaEvent &arenaEvent) {
  ClientEvent event(Type::Event);
  event.arenaEvent = arenaEvent;
  return event;
}

ClientEvent ClientEvent::Disconnect(const double uptime,
                                    const DisconnectType disconnect) {
  ClientEvent event(Type::Disconnect);
  event.uptime = uptime;
  event.disconnect = disconnect;
  return event;
}





