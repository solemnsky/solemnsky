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
/**
 * The backbone of a multiplayer game.
 */
#pragma once
#include "util/types.h"
#include "util/methods.h"
#include "participation.h"
#include <map>
#include <list>
#include <vector>

namespace sky {

struct ArenaEvent; // event.h

/**
 * Initializer type for Player's Networked implementation.
 */
struct PlayerInitializer {
  PlayerInitializer() = default; // packing
  PlayerInitializer(const PID pid, const std::string &nickname);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, pid, admin, team);
  }

  PID pid;
  std::string nickname;
  bool admin;
  Team team;
};

/**
 * Delta type for Player's Networked implementation.
 */
struct PlayerDelta {
  PlayerDelta() = default; // packing

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(nickname, admin, team);
  }

  optional<std::string> nickname;
  bool admin;
  optional<Team> team;
};

/**
 * Represents a player in the arena, with some minimal metadata.
 */
struct Player: public Networked<PlayerInitializer, PlayerDelta> {
  template<typename T>
  friend
  class Subsystem;
 private:
  // State.
  std::string nickname;
  bool admin;
  Team team;
  std::map<PID, void *> data; // this is a good and not a bad idea

 public:
  Player() = delete;
  Player(class Arena &arena, const PlayerInitializer &initializer);

  // Parameters.
  class Arena &arena;
  const PID pid;

  // Networked impl.
  void applyDelta(const PlayerDelta &delta) override;
  PlayerInitializer captureInitializer() const override;
  PlayerDelta zeroDelta() const;

  // User API.
  std::string getNickname() const;
  bool isAdmin() const;
  Team getTeam() const;

  void doAction(const Action action, const bool state);
  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f &pos, const float rot);

};

/**
 * Type-erasure for Subsystem, representing the uniform callback API.
 *
 * Subsystem callbacks do not necessarily transmit over the network! If all
 * clients need to register the same callbacks, they should by triggered by
 * Arena / Subsystem Deltas (onMode, onMapChange, etc).
 */
class SubsystemListener {
  friend class Arena;
  friend class Player;
 protected:
  // Managing player registration.
  virtual void registerPlayer(Player &player) { }
  virtual void unregisterPlayer(Player &player) { }

  // Callbacks.
  virtual void onTick(const float delta) { }
  virtual void onJoin(Player &player) { }
  virtual void onQuit(Player &player) { }
  virtual void onMode(const ArenaMode newMode) { }
  virtual void onMapChange() { }
  virtual void onAction(Player &player,
                        const Action action, const bool state) { }
  virtual void onSpawn(Player &player, const PlaneTuning &tuning,
                       const sf::Vector2f &pos, const float rot) { };

};

/**
 * The subsystem abstraction: attaches additional layers of state and logic to the game.
 */
template<typename PlayerData>
class Subsystem: public SubsystemListener {
 protected:
  const PID id; // ID the render has allocated in the Arena

  PlayerData &getPlayerData(const Player &player) const {
    return *((PlayerData *) player.data.at(id));
  }

  void setPlayerData(Player &player, PlayerData &data) {
    player.data[id] = &data;
  }

 public:
  class Arena &arena;

  Subsystem() = delete;
  Subsystem(Arena &arena);
  virtual ~Subsystem();
};

/**
 * The arena proxyLogger abstraction: callbacks on ArenaEvents.
 */
class ArenaLogger {
 protected:
  friend class Arena;
  virtual void onEvent(const ArenaEvent &event) { }

 public:
  class Arena &arena;

  ArenaLogger(Arena &arena);

};

/**
 * Initializer for Arena's Networked implementation.
 */
struct ArenaInit {
  ArenaInit() = default; // packing
  ArenaInit(const std::string &name, const MapName &map,
            const ArenaMode mode = ArenaMode::Lobby);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(players, name, motd, map, mode);
  }

  std::map<PID, PlayerInitializer> players;
  std::string name, motd;
  MapName map;
  ArenaMode mode;
};

/**
 * Delta type for Arena's Networked implementation.
 */
struct ArenaDelta: public VerifyStructure {
  enum class Type {
    Quit, Join, Delta, Motd, Mode, MapChange
  };

  ArenaDelta() = default; // packing
  ArenaDelta(const Type type);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(type);
    switch (type) {
      case Type::Quit: {
        ar(quit);
        break;
      }
      case Type::Join: {
        ar(join);
        break;
      }
      case Type::Delta: {
        ar(delta);
        break;
      }
      case Type::Motd: {
        ar(motd);
        break;
      }
      case Type::Mode: {
        ar(mode);
        break;
      }
      case Type::MapChange: {
        ar(map);
        break;
      }
    }
  }

  Type type;
  optional<PID> quit;
  optional<PlayerInitializer> join;
  optional<std::pair<PID, PlayerDelta>> delta;
  optional<std::string> motd;
  optional<ArenaMode> mode;
  optional<MapName> map;

  bool verifyStructure() const override;

  static ArenaDelta Quit(const PID pid);
  static ArenaDelta Join(const PlayerInitializer &initializer);
  static ArenaDelta Delta(const PID, const PlayerDelta &delta);
  static ArenaDelta Motd(const std::string &motd);
  static ArenaDelta Mode(const ArenaMode mode);
  static ArenaDelta MapChange(const MapName &name);
};

/**
 * The backbone of a multiplayer game. Holds Players, Subsystems, and
 * an ArenaLoggers, and exposes a small API.
 */
class Arena: public Networked<ArenaInit, ArenaDelta> {
 private:
  // Utilities.
  PID allocPid() const;
  std::string allocNickname(const std::string &requested) const;

  friend class Player;
  friend class SubsystemCaller;

  // Event logging.
  void logEvent(const ArenaEvent &event) const;

  // State.
  std::map<PID, Player> players;
  std::string name;
  std::string motd;
  MapName nextMap;
  ArenaMode mode;

  // Managing players.
  Player &joinPlayer(const PlayerInitializer &initializer);
  void quitPlayer(Player &player);

 public:
  Arena() = delete;
  Arena(const ArenaInit &initializer);

  // Subsystems and loggers.
  std::map<PID, SubsystemListener *> subsystems;
  std::vector<ArenaLogger *> loggers;

  // Networked Impl.
  void applyDelta(const ArenaDelta &delta) override;
  ArenaInit captureInitializer() const override;

  // User API.
  Player *getPlayer(const PID pid);
  void forPlayers(std::function<void(const Player &)> f) const;
  void forPlayers(std::function<void(Player &)> f);
  const std::map<PID, Player> &getPlayers() const;

  std::string getName() const;
  std::string getMotd() const;
  MapName getNextMap() const;
  ArenaMode getMode() const;

  void tick(const float delta);

  // Server-specific API.
  ArenaDelta connectPlayer(const std::string &requestedNick);

};

template<typename PlayerData>
Subsystem<PlayerData>::Subsystem(Arena &arena) :
    id(PID(smallestUnused(arena.subsystems))),
    arena(arena) {
  arena.subsystems[id] = (SubsystemListener *) this;
}

template<typename PlayerData>
Subsystem<PlayerData>::~Subsystem() {
  arena.subsystems.erase(id);
}

}
