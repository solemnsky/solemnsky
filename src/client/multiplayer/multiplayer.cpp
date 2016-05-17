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

void Multiplayer::useView(
    const sky::ArenaMode arenaMode) {
  if (view) { if (view->target == arenaMode) return; }
  switch (arenaMode) {
    case sky::ArenaMode::Lobby:
      view = std::make_unique<MultiplayerLobby>(shared, mShared);
    case sky::ArenaMode::Game:
      view = std::make_unique<MultiplayerGame>(shared, mShared);
    case sky::ArenaMode::Scoring:
      view = std::make_unique<MultiplayerScoring>(shared, mShared);
  }
}

Multiplayer::Multiplayer(ClientShared &shared,
                         const std::string &serverHostname,
                         const unsigned short serverPort) :
    Game(shared, "multiplayer"),
    mShared(shared, serverHostname, serverPort),
    view(nullptr) { }

/**
 * Game interface.
 */

void Multiplayer::onChangeSettings(const SettingsDelta &settings) {
  mShared.onChangeSettings(settings);
  if (view) view->onChangeSettings(settings);

  if (mShared.conn) {
    if (settings.nickname) {
      sky::PlayerDelta delta = mShared.conn->player.zeroDelta();
      delta.nickname = *settings.nickname;
      mShared.transmit(sky::ClientPacket::ReqPlayerDelta(delta));
      // request a nickname change
    }
  }
}

void Multiplayer::onBlur() {

}

void Multiplayer::onFocus() {

}

void Multiplayer::doExit() {
  mShared.disconnect();
}

void Multiplayer::tick(float delta) {
  mShared.poll(delta);
  if (mShared.disconnected) quitting = true;
  if (mShared.disconnecting) return;

  if (mShared.conn) {
    mShared.conn->arena.tick(delta);

    const sky::ArenaMode currentMode = mShared.conn->arena.getMode();
    if (currentMode == sky::ArenaMode::Game
        and !mShared.conn->skyHandle.isActive()) {
      useView(sky::ArenaMode::Lobby);
    } else useView(currentMode);

    view->tick(delta);
  }
}

void Multiplayer::render(ui::Frame &f) {
  if (!mShared.server) {
    if (mShared.disconnecting) {
      f.drawText({400, 400}, "Disconnecting...",
                 sf::Color::White, style.base.normalText);
    } else {
      f.drawText({400, 400}, {"Connecting..."}, sf::Color::White,
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


