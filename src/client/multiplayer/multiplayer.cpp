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
#include "multiplayer.h"
#include "client/elements/style.h"
#include "multiplayerlobby.h"
#include "multiplayergame.h"
#include "multiplayerscoring.h"

/**
 * Multiplayer.
 */

void Multiplayer::loadView(const sky::ArenaMode arenaMode) {
  switch (arenaMode) {
    case sky::ArenaMode::Lobby: {
      view = std::make_unique<MultiplayerLobby>(shared, core);
      break;
    }
    case sky::ArenaMode::Game: {
      view = std::make_unique<MultiplayerGame>(shared, core);
      break;
    }
    case sky::ArenaMode::Scoring: {
      view = std::make_unique<MultiplayerScoring>(shared, core);
      break;
    }
  }
}

Multiplayer::Multiplayer(ClientShared &shared,
                         const std::string &serverHostname,
                         const unsigned short serverPort) :
    Game(shared, "multiplayer"),
    core(shared, *this, serverHostname, serverPort),
    view(nullptr),
    hudFont(resources.getFont(ui::FontID::Default)) {
  areChildComponents({&core});
}

void Multiplayer::onConnect() {
  const auto mode = core.conn->arena.getMode();
  if (mode == sky::ArenaMode::Game) {
    if (core.conn->skyHandle.isActive()) {
      loadView(mode);
    } else {
      loadView(sky::ArenaMode::Lobby);
    }
  } else {
    loadView(mode);
  }

}

void Multiplayer::onLoadMode(const sky::ArenaMode mode) {
  if (mode != sky::ArenaMode::Game) loadView(mode);
}

void Multiplayer::onStartGame() {
  if (core.conn->arena.getMode() == sky::ArenaMode::Game) {
    loadView(sky::ArenaMode::Game);
  }
}

void Multiplayer::onEndGame() {
  if (core.conn->arena.getMode() == sky::ArenaMode::Game)
    loadView(sky::ArenaMode::Lobby);
}

void Multiplayer::onChangeSettings(const SettingsDelta &settings) {
  if (view) view->onChangeSettings(settings);
  ClientComponent::onChangeSettings(settings);
}

void Multiplayer::onBlur() {

}

void Multiplayer::onFocus() {

}

void Multiplayer::doExit() {
  core.disconnect();
}

void Multiplayer::printDebugLeft(Printer &p) {
  if (core.conn) {
    const auto &skyHandle = core.conn->skyHandle;
    const auto &arena = core.conn->arena;
    const auto &player = core.conn->player;

    p.setColor(0, 0, 255);
    p.printLn("** arena state **")
    p.setColor(255, 255, 255);
    p.printLn("name: " + arena.getName());
    p.printLn("next map: " + arena.getNextMap());
    p.printLn("your player ID: " + std::to_string(player.pid));
    p.printLn("player count: " + std::to_string(arena.getPlayers().size()));
    p.printLn("arena uptime: " + showTime(arena.getUptime()));

    p.breakLine();
    p.setColor(0, 0, 255);
    p.printLn("** network state **")
    p.setColor(255, 255, 255);
    p.printLn("inbound bandwidth (kB/s): "
                  + showKbps(core.getHost().incomingBandwidth()));
    p.printLn("outbound bandwidth (kB/s): "
                  + showKbps(core.getHost().outgoingBandwidth()));
    if (player.latencyIsCalculated()) {
      p.printLn("latency: " + showTimeDiff(player.getLatency()));
      p.printLn("clock offset: " + showTime(player.getClockOffset()));
    } else {
      p.printLn("(latency not yet calculated)");
    }
  } else {
    p.printLn("not connected...");
  }
}

void Multiplayer::printDebugRight(Printer &p) {
  if (core.conn) {
    const auto &skyHandle = core.conn->skyHandle;
    const auto &arena = core.conn->arena;
    const auto &player = core.conn->player;

    if (skyHandle.isActive()) {
      const auto &sky = skyHandle.sky.get();
      p.setColor(0, 0, 255);
      p.printLn("** sky state **")
      p.setColor(255, 255, 255);
      p.printLn("current map: " + core.conn->getSky()->getMap().name);
      p.printLn("view scale: " + std::to_string(sky.getSettings().viewScale));
      p.printLn("gravity: " + std::to_string(sky.getSettings().viewScale));

      p.breakLine();
      p.setColor(0, 0, 255);
      p.printLn("** participation state **");
      p.setColor(255, 255, 255);
      p.printLn("spawned: " + (sky.getParticipation(player).isSpawned() ?
                               "yes" : "no"));
    } else {
      p.printLn("(Sky is not instantiated.)");
    }
  }
}

bool Multiplayer::poll() {
  return core.poll();
}

void Multiplayer::tick(const TimeDiff delta) {
  core.tick(delta);

  if (core.isDisconnected()) quitting = true;
  if (core.isDisconnecting()) return;

  if (core.conn) {
    if (view) view->tick(delta);
  }
}

void Multiplayer::render(ui::Frame &f) {
  if (!core.isConnected()) {
    if (core.isDisconnecting()) {
      f.drawText({400, 400}, "Disconnecting...",
                 sf::Color::White, style.base.normalText, hudFont);
    } else {
      f.drawText({400, 400}, "Connecting...", sf::Color::White,
                 style.base.normalText, hudFont);
    }
    return;
  }

  if (view) view->render(f);
}

bool Multiplayer::handle(const sf::Event &event) {
  if (view) return view->handle(event);
  return false;
}

void Multiplayer::reset() {
  if (view) view->reset();
}

void Multiplayer::signalRead() {
  if (view) view->signalRead();
}

void Multiplayer::signalClear() {
  if (view) view->signalClear();
}


