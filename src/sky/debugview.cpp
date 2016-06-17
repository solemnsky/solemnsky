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
                     const optional<PID> &player) :
    Subsystem(arena),
    skyHandle(skyHandle),
    playerID(player) {}

void DebugView::printArenaReport(Printer &p) const {
  p.printTitle("Arena");
  p.printLn("getName(): " + arena.getName());
  p.printLn("getNextMap(): " + arena.getNextMap());
  p.printLn("getPlayers().size(): "
                + std::to_string(arena.getPlayers().size()));
  p.printLn("getUptime(): " + printTime(arena.getUptime()));

  if (playerID) {
    p.breakLine();
    p.printTitle("Player");

    p.printLn("PID: " + std::to_string(playerID.get()));
    if (auto player = arena.getPlayer(playerID.get())) {
      p.printLn("latencyIsCalculated(): "
                    + printBool(player->latencyIsCalculated()));
      if (player->latencyIsCalculated()) {
        p.printLn("getLatency(): " + printTimeDiff(player->getLatency()));
        p.printLn("getClockOffset(): " + printTime(player->getClockOffset()));
      }
    } else {
      p.setColor(255, 0, 0);
      p.printLn("ERROR: PID not associated with Player in Arena!");
      p.setColor(255, 255, 255);
    }
  }
}

void DebugView::printSkyReport(Printer &p) const {
  p.printTitle("SkyHandle");
  p.printLn("isActive(): " + printBool(skyHandle.isActive()));
  p.printLn("loadingErrored(): " + printBool(skyHandle.loadingErrored()));
  if (skyHandle.isActive()) {
    const auto &sky = skyHandle.sky.get();
    p.printLn("getMap().name: " + sky.getMap().name);
    p.printLn("getSettings().viewScale: "
                  + printFloat(sky.getSettings().viewScale));
    p.printLn("getSettings().gravity: "
                  + printFloat(sky.getSettings().viewScale));

    p.breakLine();
    if (playerID) {
      p.printTitle("Participation");
      if (auto player = arena.getPlayer(playerID.get())) {
        const auto &participation = sky.getParticipation(*player);
        p.printLn("isSpawned(): " + printBool(participation.isSpawned()));
        p.printLn("props.size(): "
                      + std::to_string(participation.props.size()));
      } else {
        p.setColor(255, 0, 0);
        p.printLn("ERROR: PID not associated with Player in Arena!");
        p.setColor(255, 255, 255);
      }
    }
  }

}

}
