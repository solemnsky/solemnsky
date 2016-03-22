/**
 * A component that attaches to an Arena and augments its functionality.
 */
#pragma once

namespace sky {

class Subsystem {
 protected:
  class Arena *arena;
  friend class Arena;

  virtual void onTick(const float delta);
  virtual void onJoin(struct Player &player);
  virtual void onQuit(struct Player &player);

 public:
  Subsystem() = delete;
  Subsystem(Arena *arena);

};

}