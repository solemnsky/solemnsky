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
#include "event.h"

namespace sky {

/**
 * ArenaEvent.
 */

ArenaEvent::ArenaEvent(const Type type) : type(type) { }

void ArenaEvent::print(Printer &p) const {
  p.setColor(255, 255, 255);
  switch (type) {
    case Type::Join: {
      p.print(inQuotes(name.get()) + " joined the game!");
      break;
    }
    case Type::Quit: {
      p.print(inQuotes(name.get()) + " left the game!");
      break;
    }
    case Type::NickChange: {
      p.print(inQuotes(name.get()) + " changed name to "
                  + inQuotes(newName.get()));
      break;
    }
    case Type::TeamChange: {
      p.print(inQuotes(name.get()) + " changed team from "
                  + std::to_string(oldTeam.get())
                  + " to " + std::to_string(newTeam.get()));
      break;
    }
    case Type::ModeChange: {
      p.setColor(0, 255, 0);
      switch (mode.get()) {
        case sky::ArenaMode::Lobby: {
          p.print("** Entered lobby. **");
          break;
        }
        case sky::ArenaMode::Game: {
          p.print("** Entered game. **");
          break;
        }
        case sky::ArenaMode::Scoring: {
          p.print("** Entered scoring. **");
          break;
        }
      }
      break;
    }
    case Type::MapChange: {
      p.setColor(0, 255, 0);
      p.print("** Changed map to " + inQuotes(map.get()) + ". **");
      break;
    }
  }
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

ArenaEvent ArenaEvent::MapChange(const MapName &map) {
  ArenaEvent event(Type::MapChange);
  event.map = map;
  return event;
}

}

/**
 * ServerEvent.
 */

ServerEvent::ServerEvent(const Type type) : type(type) { }

void ServerEvent::print(Printer &p) const {
  p.setColor(255, 255, 255);
  switch (type) {
    case Type::Start: {
      p.print("Started " + inQuotes(stringData.get())
                  + " on port " + std::to_string(port.get()));
      break;
    }
    case Type::Event: {
      arenaEvent->print(p);
      break;
    }
    case Type::Stop: {
      p.print("Stopped server after " + std::to_string(uptime.get())
                  + " seconds of uptime");
      break;
    }
    case Type::Connect: {
      p.print("Client connected: " + inQuotes(stringData.get()));
      break;
    }
    case Type::Disconnect: {
      p.print("Client disconnected: " + inQuotes(stringData.get()));
      break;
    }
    case Type::RConIn: {
      p.print("[rcon] <- " + stringData.get());
      break;
    }
    case Type::RConOut: {
      p.print("[rcon] -> " + stringData.get());
      break;
    }
  }
}

ServerEvent ServerEvent::Start(const Port port,
                               const std::string &name) {
  ServerEvent event(Type::Start);
  event.port = port;
  event.stringData = name;
  return event;
}

ServerEvent ServerEvent::Event(const sky::ArenaEvent &arenaEvent) {
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
  event.stringData = name;
  return event;
}

ServerEvent ServerEvent::Disconnect(const std::string &name) {
  ServerEvent event(Type::Disconnect);
  event.stringData = name;
  return event;
}

ServerEvent ServerEvent::RConIn(const std::string &command) {
  ServerEvent event(Type::RConIn);
  event.stringData = command;
  return event;
}

ServerEvent ServerEvent::RConOut(const std::string &response) {
  ServerEvent event(Type::RConOut);
  event.stringData = response;
  return event;
}

/**
 * ClientEvent.
 */

ClientEvent::ClientEvent(const Type type) : type(type) { }

void ClientEvent::print(Printer &p) const {
  p.setColor(255, 255, 255);
  switch (type) {
    case Type::Connect: {
      p.setColor(255, 0, 0);
      p.print("Connected to " + inQuotes(name.get())
                  + " @ " + ipAddr->toString());
      break;
    }
    case Type::Event: {
      arenaEvent->print(p);
      break;
    }
    case Type::Disconnect: {
      switch (disconnect.get()) {
        case sky::DisconnectType::Graceful: {
          p.print("Disconnected from server gracefully.");
          break;
        }
        case sky::DisconnectType::Timeout: {
          p.print("Disconnected from server due to timeout.");
          break;
        }
      }
    }
    case Type::Chat: {
      p.setColor(255, 0, 0);
      p.print("[chat] ");
      p.setColor(255, 255, 255);
      p.print(name.get() + ": " + message.get());
      break;
    }
    case Type::Broadcast: {
      p.print("[broadcast] " + message.get());
      break;
    }
    case Type::RConCommand: {
      p.setColor(0, 0, 255);
      p.print("[rcon] -> " + message.get());
      break;
    }
    case Type::RConResponse: {
      p.setColor(0, 0, 255);
      p.print("[rcon] <- " + message.get());
      break;
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

ClientEvent ClientEvent::Event(const sky::ArenaEvent &arenaEvent) {
  ClientEvent event(Type::Event);
  event.arenaEvent = arenaEvent;
  return event;
}

ClientEvent ClientEvent::Disconnect(const double uptime,
                                    const sky::DisconnectType disconnect) {
  ClientEvent event(Type::Disconnect);
  event.uptime = uptime;
  event.disconnect = disconnect;
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

ClientEvent ClientEvent::RConCommand(const std::string &command) {
  ClientEvent event(Type::RConCommand);
  event.message = command;
  return event;
}

ClientEvent ClientEvent::RConResponse(const std::string &response) {
  ClientEvent event(Type::RConResponse);
  event.message = response;
  return event;
}
