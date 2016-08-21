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
#include "debugview.hpp"

namespace sky {

/**
 * DebugInfo.
 */

DebugView::DebugView(Arena &arena,
                     const SkyHandle &skyHandle,
                     const optional<PID> player) :
    Subsystem(arena),
    skyHandle(skyHandle),
    playerID(player) { }

void DebugView::printArenaReport(Printer &p) const {
  p.printTitle("Arena");
  p.printLn("getName(): " + arena.getName());
  p.printLn("getNextEnv(): " + arena.getNextEnv());
  p.printLn("getPlayers().size(): "
            + std::to_string(arena.getPlayers().size()));
  p.printLn("getUptime(): " + printTime(arena.getUptime()));

  if (playerID) {
    p.breakLine();
    p.printTitle("Player");

    p.printLn("PID: " + std::to_string(playerID.get()));
    if (auto player = arena.getPlayer(playerID.get())) {
      if (const auto stats = player->getConnectionStats()) {
        p.printLn("stats.latency: " + printTimeDiff(stats->latency));
        p.printLn("stats.offset: " + printTime(stats->offset));
        p.printLn("stats.flow.averageWait: " + printTimeDiff(stats->flow.averageWait));
        p.printLn("stats.flow.inputJitter: " + printTimeDiff(stats->flow.inputJitter));
        p.printLn("stats.flow.outputJitter: " + printTimeDiff(stats->flow.outputJitter));
      } else {
        p.printLn("(Connection statistics not initialized.)");
      }
      p.printLn("player->getTeam(): " +  std::to_string(player->getTeam()));
      p.printLn("player->getNickname(): " + inQuotes(player->getNickname()));
    } else {
      p.setColor(255, 0, 0);
      p.printLn("ERROR: PID not associated with Player in Arena!");
      p.setColor(0, 0, 0);
    }
  }
}

void DebugView::printSkyReport(Printer &p) const {
  p.printTitle("SkyHandle");
  p.printLn("bool(getSky()): " + printBool(bool(skyHandle.getSky())));
  p.printLn("bool(getEnvironment()): "
            + printBool(bool(skyHandle.getEnvironment())));

  if (const auto sky = skyHandle.getSky()) {
    p.printLn("environment.url: " + skyHandle.getEnvironment()->url);
    p.printLn("sky.settings.getGravity(): "
              + printFloat(sky->settings.getGravity()));
    p.printLn("sky.settings.getViewScale(): "
              + printFloat(sky->settings.getViewscale()));

    p.breakLine();
    if (playerID) {
      p.printTitle("Participation");
      if (auto player = arena.getPlayer(playerID.get())) {
        const auto &participation = sky->getParticipation(*player);
        const bool spawned = participation.isSpawned();

        p.printLn("isSpawned(): " + printBool(spawned));
        if (spawned) {
          const auto &plane = participation.plane.get();
          p.printLn("state.physical.pos: " +
                    printVector(plane.getState().physical.pos));
        }
      } else {
        p.setColor(255, 0, 0);
        p.printLn("ERROR: PID not associated with Player in Arena!");
        p.setColor(0, 0, 0);
      }
    }
  }
}

}
