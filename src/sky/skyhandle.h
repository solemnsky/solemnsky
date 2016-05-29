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
 * A potential Sky instantiation.
 */
#pragma once
#include "sky.h"

namespace sky {

/**
 * Initializer for SkyHandle.
 */

struct SkyHandleInit {
  SkyHandleInit() = default;
  explicit SkyHandleInit(const SkyInit &initializer);

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(initializer);
  }

  bool verifyStructure() const;

  optional<SkyInit> initializer;
};

/**
 * Delta for SkyHandle.
 */

struct SkyHandleDelta {
  SkyHandleDelta() = default;

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(initializer, delta);
  }

  bool verifyStructure() const;

  optional<SkyInit> initializer;
  optional<SkyDelta> delta;

  // Respecting client authority.
  SkyHandleDelta respectAuthority(const Player &player) const;

};

/**
 * Wraps an optional<Sky>, binding it to the arena when the game is in session.
 * Also wraps its Networked implementation.
 */
class SkyHandle
    : public Subsystem<Nothing>,
      public Networked<SkyHandleInit, SkyHandleDelta> {
 private:
  // State.
  optional<Sky> sky;
  bool skyIsNew;

 public:
  SkyHandle(class Arena &parent, const SkyHandleInit &initializer);

  // Networking.
  SkyHandleInit captureInitializer() const override final;
  SkyHandleDelta collectDelta();
  void applyDelta(const SkyHandleDelta &delta) override final;

  // User API.
  void start();
  void stop();

  const optional<Sky> &getSky() const;
  bool isActive() const;

};

}

