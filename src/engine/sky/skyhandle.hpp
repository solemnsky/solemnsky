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
#include "sky.hpp"
#include "engine/environment/environment.hpp"

namespace sky {

using SkyHandleInit = optional<EnvironmentURL>;
using SkyHandleDelta = optional<EnvironmentURL>;

/**
 * Wraps an optional<Sky>, binding it to the arena when the game is in session.
 * Also wraps its Networked implementation.
 */
class SkyHandle
    : public Subsystem<Nothing>,
      public Networked<SkyHandleInit, SkyHandleDelta> {
 private:
  // Delta collection state.
  bool envIsNew;

 public:
  SkyHandle(class Arena &parent, const SkyHandleInit &initializer);

  // Wrapped state: environment and sky.
  optional<Environment> environment;
  optional<Sky> sky;

  // Networking.
  SkyHandleInit captureInitializer() const override final;
  SkyHandleDelta collectDelta();
  void applyDelta(const SkyHandleDelta &delta) override final;

  // User API.
  void instantiateSky(const SkyInit &skyInit);
  void stop();

  bool isActive() const;
  bool loadingErrored() const;

};

}

