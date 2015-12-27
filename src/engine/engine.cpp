#include "engine.h"

namespace sky {

void Player::kill() {
  alive = false;
}

void Player::spawn(const PlaneState state) {
  plane = Plane(engine, state);
}

}
