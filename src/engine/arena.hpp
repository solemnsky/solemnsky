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
 * The backbone of the engine. Extensible through the Subsystem abstraction.
 */
#pragma once
#include <map>
#include <list>
#include <vector>
#include "util/types.hpp"
#include "util/methods.hpp"
#include "player.hpp"
#include "role.hpp"

namespace sky {

struct ArenaEvent; // event.h

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
  friend class SubsystemCaller;
  friend class Sky;
 protected:
  virtual ~SubsystemListener() { }

  // Managing player registration.
  virtual void registerPlayer(Player &player);
  virtual void unregisterPlayer(Player &player);

  // Callbacks.
  virtual void onPoll(const TimeDiff delta);
  virtual void onTick(const TimeDiff delta);

  virtual void onJoin(Player &player);
  virtual void onQuit(Player &player);
  virtual void onMode(const ArenaMode newMode);
  virtual void onMapChange();
  virtual void onDelta(Player &player,
                       const PlayerDelta &delta);
  virtual void onAction(Player &player,
                        const Action action, const bool state);
  virtual void onSpawn(Player &player);
  virtual void onKill(Player &player);

  virtual void onStartGame();
  virtual void onEndGame();

};

/**
 * Set of callback triggers for Subsystems to call.
 */
class SubsystemCaller {
 private:
  Arena &arena;

 public:
  SubsystemCaller(Arena &arena);

  void doSpawn(Player &player);
  void doKill(Player &player);

  void doStartGame();
  void doEndGame();

};

/**
 * The subsystem abstraction: attaches additional layers of state and logic to the game.
 */
template<typename PlayerData>
class Subsystem: public SubsystemListener {
 protected:
  SubsystemCaller &caller;
  const PID id; // ID the render has allocated in the Arena

  PlayerData &getPlayerData(const Player &player) const {
    return *((PlayerData *) player.data.at(id));
  }

  void setPlayerData(Player &player, PlayerData &data) {
    player.data[id] = &data;
  }

 public:
  Arena &arena;
  Role &role;

  Subsystem() = delete;
  Subsystem(Arena &arena);
  virtual ~Subsystem();

};

/**
 * The arena logger abstraction: callbacks on ArenaEvents.
 */
class ArenaLogger {
 protected:
  friend class Arena;
  virtual void onEvent(const ArenaEvent &event);

 public:
  class Arena &arena;

  ArenaLogger() = delete;
  ArenaLogger(Arena &arena);
  virtual ~ArenaLogger() { }

};

/**
 * Initializer for Arena's Networked implementation.
 */
struct ArenaInit {
  ArenaInit() = default; // packing
  ArenaInit(const std::string &name,
            const EnvironmentURL &environment,
            const ArenaMode mode = ArenaMode::Lobby,
            const int teamCount = 2);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(players, name, motd, environment, mode);
  }

  std::map<PID, PlayerInitializer> players;
  std::string name, motd;
  EnvironmentURL environment;
  ArenaMode mode;
  int teamCount;
};

/**
 * Delta type for Arena's Networked implementation.
 */
struct ArenaDelta: public VerifyStructure {
  enum class Type {
    Quit,
    Join,
    Delta,
    ResetEnvLoad,
    Motd,
    Mode,
    EnvChange,
    TeamCount
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
        ar(playerDeltas);
        break;
      }
      case Type::ResetEnvLoad: {
        break;
      };
      case Type::Motd: {
        ar(motd);
        break;
      }
      case Type::Mode: {
        ar(mode);
        break;
      }
      case Type::EnvChange: {
        ar(environment);
        break;
      }
      case Type::TeamCount: {
        ar(teamCount);
        break;
      }
    }
  }

  Type type;
  optional<PID> quit;
  optional<PlayerInitializer> join;
  optional<std::map<PID, PlayerDelta>> playerDeltas;
  optional<std::string> motd;
  optional<ArenaMode> mode;
  optional<EnvironmentURL> environment;
  optional<int> teamCount;

  bool verifyStructure() const override;

  static ArenaDelta Quit(const PID pid);
  static ArenaDelta Join(const PlayerInitializer &initializer);
  static ArenaDelta Delta(const PID, const PlayerDelta &playerDelta);
  static ArenaDelta Delta(const std::map<PID, PlayerDelta> &playerDeltas);
  static ArenaDelta ResetEnvLoad();
  static ArenaDelta Motd(const std::string &motd);
  static ArenaDelta Mode(const ArenaMode mode);
  static ArenaDelta EnvChange(const EnvironmentURL &name);
  static ArenaDelta TeamCount(const int &count);

};

/**
 * The backbone of a multiplayer game. Holds Players, Subsystems, and
 * an ArenaLoggers, and exposes a small API.
 */
class Arena: public Networked<ArenaInit, ArenaDelta> {
  friend class Player;
  friend class SubsystemCaller;
 private:
  // PID and nickname allocation.
  PID allocPid() const;
  std::string allocNickname(const std::string &cleanReq,
                            const optional<PID> ignorePid = {}) const;

  // Placement on the network.
  const optional<PID> playerOwnership; // We're owned by a player with a certain PID.
  bool sandboxed; // We're in an offline sandbox.

  // Event logging.
  void logEvent(const ArenaEvent &event) const;

  // State.
  std::map<PID, Player> players;
  std::string name;
  std::string motd;
  EnvironmentURL nextEnv;
  ArenaMode mode;
  Time uptime;
  int teamCount;

  // Managing players.
  Player &joinPlayer(const PlayerInitializer &initializer);
  void quitPlayer(Player &player);
  void applyPlayerDelta(const PID pid, const PlayerDelta &playerDelta);

 public:
  Arena() = delete;
  Arena(const ArenaInit &initializer, const optional<PID> isClient = {}, const bool isSandbox = false);

  Role role;

  // Attachments.
  SubsystemCaller subsystemCaller;
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

  const std::string &getName() const;
  const std::string &getMotd() const;
  const EnvironmentURL &getNextEnv() const;
  ArenaMode getMode() const;
  Time getUptime() const;
  int getTeamCount() const;

  // Server API.
  Team autojoinTeam() const;

  // Ticking / polling.
  void tick(const TimeDiff delta);
  void poll(const TimeDiff delta);

  // Server-specific API.
  ArenaDelta connectPlayer(const std::string &requestedNick);
  std::string allocNewNickname(const Player &player,
                               const std::string &requestedNick);

};

template<typename PlayerData>
Subsystem<PlayerData>::Subsystem(Arena &arena) :
    caller(arena.subsystemCaller),
    id(PID(smallestUnused(arena.subsystems))),
    arena(arena),
    role(arena.role) {
  arena.subsystems[id] = (SubsystemListener *) this;
}

template<typename PlayerData>
Subsystem<PlayerData>::~Subsystem() {
  arena.subsystems.erase(id);
}

}
