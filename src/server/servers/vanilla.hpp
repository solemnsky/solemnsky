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
 * Default multiplayer server, with a cool rcon and a lazy tdm mode.
 */
#pragma once
#include "server/server.hpp"
#include "util/types.hpp"

class VanillaServer: public Server<Nothing> {
 private:
  // Subroutines.
  void tickGame(const TimeDiff delta, sky::Sky &sky);

 protected:
  // Subsystem callbacks.
  void onTick(const TimeDiff delta) override final;

  //Collision
  void onBeginContact(const sky::BodyTag &body1, const sky::BodyTag &body2) override final;
  void onEndContact(const sky::BodyTag &body1, const sky::BodyTag &body2) override final;
  bool enableContact(const sky::BodyTag &body1, const sky::BodyTag &body2) override final;

  // Server callbacks.
  void onPacket(ENetPeer *const client,
                sky::Player &player,
                const sky::ClientPacket &packet) override final;

 public:
  VanillaServer(ServerShared &shared);

};
