#include "event.h"

/**
 * ArenaEvent.
 */

ArenaEvent::ArenaEvent(const Type type) : type(type) { }

void ArenaEvent::print(Printer &p) const {
  switch (type) {
    case Type::Chat:
      p.print("[chat] " + *name + ": " + *message);
    case Type::Broadcast:
      p.print("[server] " + *message);
    case Type::Join:
      p.print(inQuotes(name.get()) + " joined the game!");
    case Type::Quit:
      p.print(inQuotes(name.get()) + " left the game!");
    case Type::NickChange:
      p.print(inQuotes(name.get()) + " changed name to "
                  + inQuotes(newName.get()));
    case Type::TeamChange:
      p.print(inQuotes(name.get()) + " changed team from "
                  + std::to_string(oldTeam.get())
                  + " to " + std::to_string(newTeam.get()));
    case Type::ModeChange: {
      switch (mode.get()) {
        case sky::ArenaMode::Lobby:
          p.print("** Entered lobby. **");
        case sky::ArenaMode::Game:
          p.print("** Began game on " + *name + " **");
        case sky::ArenaMode::Scoring:
          p.print("** Entered scoring. **");
      }
    }
  }
}

std::string ArenaEvent::printString() const {
  StringPrinter p;
  print(p);
  return p.getString();
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
                                  const sky::Team oldTeam,
                                  const sky::Team newTeam) {
  ArenaEvent event(Type::TeamChange);
  event.name = name;
  event.oldTeam = oldTeam;
  event.newTeam = newTeam;
  return event;
}

ArenaEvent ArenaEvent::ModeChange(const sky::ArenaMode mode) {
  ArenaEvent event(Type::ModeChange);
  event.mode = mode;
  return event;
}

/**
 * ServerEvent.
 */

ServerEvent::ServerEvent(const Type type) : type(type) { }

void ServerEvent::print(Printer &p) const {
  switch (type) {
    case Type::Start:
      p.print("Started " + inQuotes(name.get())
                  + " on port " + std::to_string(port.get()));
    case Type::Event:
      arenaEvent->print(p);
    case Type::Stop:
      p.print("Stopped server after " + std::to_string(uptime.get())
                  + " seconds of uptime");
    case Type::Connect:
      p.print("Client connected: " + inQuotes(name.get()));
    case Type::Disconnect:
      p.print("Client disconnected: " + inQuotes(name.get()));
  }
}

std::string ServerEvent::printString() const {
  StringPrinter p;
  print(p);
  return p.getString();
}

ServerEvent ServerEvent::Start(const Port port,
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

ServerEvent ServerEvent::Connect(const std::string &name) {
  ServerEvent event(Type::Connect);
  event.name = name;
  return event;
}

ServerEvent ServerEvent::Disconnect(const std::string &name) {
  ServerEvent event(Type::Disconnect);
  event.name = name;
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





