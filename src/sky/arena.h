/**
 * The backbone of a multiplayer game.
 */
#pragma once
#include "util/types.h"
#include "util/methods.h"
#include "plane.h"
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
  optional<Team> team; // 0 is spectator, 1 left, 2 right
};

/**
 * Represents a player in the arena, with some minimal metadata.
 */
struct Player: public Networked<PlayerInitializer, PlayerDelta> {
 private:
  class Arena &arena;

  // State: managed through the Networked API.
  std::string nickname;
  bool admin;
  Team team;

 public:
  Player() = delete;
  Player(Arena &arena, const PlayerInitializer &initializer);

  const PID pid;
  std::vector<void *> data;

  /**
   * Top-level API.
   */
  // Networked API.
  void applyDelta(const PlayerDelta &delta) override;
  PlayerInitializer captureInitializer() const override;
  PlayerDelta zeroDelta() const;

  // Accessing state.
  std::string getNickname() const;
  bool isAdmin() const;
  Team getTeam() const;

  // Subsystem triggers.
  void doAction(const Action action, const bool state);
  void spawn(const PlaneTuning &tuning,
             const sf::Vector2f &pos, const float rot);
};

/**
 * The subsystem abstraction: attaches state to players and has various
 * callbacks. Some callbacks can be triggered by subsystems through
 * SubsystemCaller.
 *
 * Subsystem callbacks do not necessarily transmit over the network! If all
 * clients need to register the same callbacks, they should by triggered by
 * Arena / Subsystem Deltas (onMode, onMapChange, etc).
 */
class Subsystem {
 protected:
  // subsystem callbacks can be triggered by the Arena, the SubsystemCaller (for
  // subsystems to trigger events), or the Player (which is essentially an
  // extension of the Arena API)
  friend class Arena;
  friend class Player;

  const PID id; // ID the render has allocated in the Arena

  template<typename Data>
  Data &getPlayerData(const Player &player) const {
    return *((Data *) player.data.at(id));
  }

  // managing player registration
  virtual void registerPlayer(Player &player) = 0;
  virtual void unregisterPlayer(Player &player) = 0;

  // arena-triggered callbacks
  virtual void onTick(const float delta) { }
  virtual void onJoin(Player &player) { }
  virtual void onQuit(Player &player) { }
  virtual void onMode(const ArenaMode newMode) { }
  virtual void onMapChange() { }
  virtual void onAction(Player &player,
                        const Action action, const bool state) { }
  virtual void onSpawn(Player &player, const PlaneTuning &tuning,
                       const sf::Vector2f &pos, const float rot) { };

 public:
  class Arena &arena;

  Subsystem() = delete;
  Subsystem(Arena &arena);
  // a subsystem has to make sure to manually register the arena's players on
  // construction!
};

/**
 * The arena logger abstraction: callbacks on ArenaEvents.
 */
class ArenaLogger {
 protected:
  friend class Arena;
  virtual void onEvent(const ArenaEvent &event) { }

 public:
  class Arena &arena;

  ArenaLogger() = delete;
  ArenaLogger(Arena &arena);

};

/**
 * Initializer type for Arena's Networked implementation.
 */
struct ArenaInitializer {
  ArenaInitializer() = default; // packing
  ArenaInitializer(const std::string &name, const MapName &map);

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
class Arena: public Networked<ArenaInitializer, ArenaDelta> {
 private:
  // Utilities.
  PID allocPid() const;
  std::string allocNickname(const std::string &requested) const;

  friend class Player;
  friend class SubsystemCaller;

  // Event logging.
  void logEvent(const ArenaEvent &event) const;

  // State: managed through the Networked API.
  std::map<PID, Player> players;
  std::string name;
  std::string motd;
  MapName map;
  ArenaMode mode;

  // Managing players.
  Player &joinPlayer(const PlayerInitializer &initializer);
  void quitPlayer(Player &player);

 public:
  Arena() = delete;
  Arena(const ArenaInitializer &initializer);

  // Subsystems and loggers.
  std::vector<Subsystem *> subsystems;
  std::vector<ArenaLogger *> loggers;

  /**
   * Top-level API.
   */
  // Networked interface.
  void applyDelta(const ArenaDelta &delta) override;
  ArenaInitializer captureInitializer() const override;

  // Accessing state.
  Player *getPlayer(const PID pid);
  void forPlayers(std::function<void(const Player &)> f) const;
  void forPlayers(std::function<void(Player &)> f);

  std::string getName() const;
  std::string getMotd() const;
  MapName getMap() const;
  ArenaMode getMode() const;

  // Connecting players (for the server).
  ArenaDelta connectPlayer(const std::string &requestedNick);

  // Simulating.
  void tick(const float delta);
};

}
