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
 * Server-side subsystem to manage the timely application of inputs from clients.
 */
#pragma once
#include "server/servershared.hpp"
#include "engine/flowcontrol.hpp"

namespace sky {

/**
 * Manager for the ParticipationInput stream produced by one player.
 */
class PlayerInputManager {
  friend class SkyInputManager;
 private:
  Player &player;
  ServerShared &shared;
  Arena &arena;

  FlowControl<sky::ParticipationInput> inputControl;

 protected:
  // SkyInputManager impl.

  void cacheInput(const Time timestamp, const sky::ParticipationInput &input);
  void poll();


 public:
  PlayerInputManager(sky::Player &player, ServerShared &shared);

  // Stat collection.
  inline FlowStats getStats() const {
    return inputControl.getStats();
  }

};

class SkyInputManager : public Subsystem<PlayerInputManager> {
 private:
  ServerShared &shared;
  std::map<PID, PlayerInputManager> inputManagers;

 protected:
  // Subsystem impl.
  virtual void registerPlayer(Player &player) override final;
  virtual void unregisterPlayer(Player &player) override final;
  virtual void onPoll() override final;

 public:
  SkyInputManager(ServerShared &shared);

  void receive(sky::Player &player, const Time timestamp,
               const sky::ParticipationInput &input);

};

}
