#include "event.h"

/**
 * ServerEvent.
 */

ServerEvent::ServerEvent(const ServerEvent::Type type) : type(type) { }

std::string ServerEvent::printReadable() const {
  std::stringstream output;
  switch (type) {
    case Type::Start: {
      output << "Starting server on port ";
      output << start->first;
      output << "; Arena name is: ";
      output << inQuotes(start->second.name);
      break;
    }
    case Type::Connect: {
      output << "Client connected from IP ";
      output << ipAddr->toString();
      break;
    }
    case Type::Join: {
      output << "Client joined arena. PID: ";
      output << playerInitializer->pid;
      output << " Nickname: ";
      output << playerInitializer->nickname;
      break;
    }
    case Type::Delta: {
      output << ""
    }
    case Type::Quit:
      break;
    case Type::Message:
      break;
    case Type::Stop:
      break;
  }
  return output.str();
}

ServerEvent ServerEvent::Start(const unsigned char port,
                               const sky::ArenaInitializer &initializer) {
  ServerEvent event(Type::Start);
  return event;
}

ServerEvent ServerEvent::Connect(const sf::IpAddress &ipAddr) {
  ServerEvent event(Type::Connect);
  event.ipAddr = ipAddr;
  return event;
}

ServerEvent ServerEvent::Join(const sf::IpAddress &ipAddr,
                              const sky::PlayerInitializer &initializer) {
  ServerEvent event(Type::Join);
  event.ipAddr = ipAddr;
  event.playerInitializer = initializer;
  return event;
}

ServerEvent ServerEvent::Delta(const sky::ArenaDelta &delta) {
  ServerEvent event(Type::Delta);
  return event;
}

ServerEvent ServerEvent::Message(const sky::ServerMessage &message) {
  ServerEvent event(Type::Message);
  return event;
}

ServerEvent ServerEvent::Quit(const sf::IpAddress &ipAddr,
                              const sky::PlayerInitializer &initializer) {
  ServerEvent event(Type::Quit);
  return event;
}


ServerEvent ServerEvent::Stop() {
  return ServerEvent(Type::Stop);
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
  event.name = name;
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
