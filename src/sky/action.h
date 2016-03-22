/**
 * Actions players can make to control their planes.
 */
#pragma once

namespace sky {

/**
 * Clients, through their user-specified key bindings, can generate
 * sky::Actions. These actions are passed into the sky through sky::Player::
 */
struct Action {
 public:
  enum class Type {
    Thrust,
    Reverse,
    Left,
    Right,
    Primary,
    Secondary,
    Special,
    Suicide
  };
 private:
  Action(const Type type,
};

}
