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
 * Base interfaces for our gamestate-synchronizing infrastructure.
 */
#include "util/types.hpp"
#pragma once

/**
 * A type that can be synchronized over the network.
 *
 * When a client connects, the server sends an Init. From there on,
 * the server potentially sends Deltas every time it needs to be updated.
 *
 * Deltas can be constructed manually (see Arena) or collected automatically
 * (see the AutoNetworked interface and Sky)
 */
template<typename Init, typename Delta>
class Networked {
 public:
  // Dependant types for simplifying templates.
  using InitType = Init;
  using DeltaType = Delta;

  // It is obligatory to construct a Networked object with its
  // initializer type; this guarantees an orthogonal interface.
  Networked() = delete;
  Networked(const Init &) { }

  virtual void applyDelta(const Delta &) = 0;
  virtual Init captureInitializer() const = 0;

};

/**
 * A Networked that collects its Deltas automatically.
 */
template<typename Init, typename Delta>
class AutoNetworked: public Networked<Init, Delta> {
 public:
  AutoNetworked(const Init &init) : Networked<Init, Delta>(init) { }

  virtual optional<Delta> collectDelta() = 0;

};
