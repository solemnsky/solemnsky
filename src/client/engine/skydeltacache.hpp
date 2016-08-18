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
 * Clientside subsystem to manage the timely application of SkyDeltas from the server.
 */
#pragma once
#include "engine/arena.hpp"
#include "engine/sky/skyhandle.hpp"
#include "engine/flowcontrol.hpp"

namespace sky {

class SkyDeltaCache: public Subsystem<Nothing> {
 private:
  SkyHandle &skyHandle;
  FlowControl<SkyDelta> deltaControl;

 protected:
  // Subsystem impl.
  virtual void onPoll() override final;

 public:
  SkyDeltaCache(Arena &arena, SkyHandle &skyHandle);

  void receive(const Time timestamp, const SkyDelta &delta);

};

}
