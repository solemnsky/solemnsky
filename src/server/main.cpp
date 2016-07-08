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
 * Server top-level.
 */
#include "server.hpp"
#include "servers/vanilla.hpp"

int main() {
  // TODO: commandline arguments

  // and He said,
  ServerExec(4242, sky::ArenaInit("my special server", "funnelpark"),
             [](ServerShared &shared) {
               return std::make_unique<VanillaServer>(shared);
      }).run();
  // and lo, there appeared a server
}
