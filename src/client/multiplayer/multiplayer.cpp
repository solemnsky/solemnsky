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
    view(nullptr) {
  areChildComponents({&core});
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
    loadView(sky::ArenaMode::Scoring);
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

void Multiplayer::printDebug(Printer &p) {
  if (core.conn) {
    const bool active = core.conn->skyHandle.isActive();
    p.printLn(std::string("game active: ") + (active ? "yes" : "no"));
    if (active)
      p.printLn("current map: " + core.conn->getSky()->getMap().name);
    p.printLn("next map: " + core.conn->arena.getNextMap());
    p.printLn("player ID: " +
        std::to_string(core.conn->player.pid));
    p.printLn("player count: " +
        std::to_string(core.conn->arena.getPlayers().size()));

    p.breakLine();
    p.printLn("inbound bandwidth: " +
        std::to_string(core.getHost().incomingBandwidth()));
    p.printLn("outbound bandwidth: " +
        std::to_string(core.getHost().outgoingBandwidth()));
  } else {
    p.printLn("not connected...");
  }
}

void Multiplayer::poll(const float delta) {
  core.poll(delta);
}

void Multiplayer::tick(const float delta) {
  core.tick(delta);
  if (core.isDisconnected()) quitting = true;
  if (core.isDisconnecting()) return;

  if (core.conn) {
    core.conn->arena.tick(delta);
    view->tick(delta);
  }
}

void Multiplayer::render(ui::Frame &f) {
  if (!core.isConnected()) {
    if (core.isDisconnecting()) {
      f.drawText({400, 400}, "Disconnecting...",
                 sf::Color::White, style.base.normalText);
    } else {
      f.drawText({400, 400}, "Connecting...", sf::Color::White,
                 style.base.normalText);
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


