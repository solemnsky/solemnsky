/**
 * A component that attaches to an Arena and augments its functionality.
 */
#pragma once

namespace sky {

class Subsystem {
 private:
  class Arena *arena;

 protected:
  friend class Arena;

  virtual void onTick(const float delta);
  virtual void onJoin(const class Player &player);
  virtual void onQuit(const class Player &player);

 public:
  Subsystem() = delete;
  Subsystem(Arena *arena);

};

}