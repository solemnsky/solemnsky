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
#include "multiplayergamehandle.hpp"
#include "client/elements/style.hpp"

/**
 * MultiplayerGameHandle.
 */

MultiplayerGameHandle::MultiplayerGameHandle(
    ClientShared &shared, MultiplayerCore &core) :
    MultiplayerView(shared, core),
    splash(shared, core) {
  assert(conn.skyHandle.getEnvironment());
}

void MultiplayerGameHandle::poll() {
  if (gameView) {
    gameView->poll();
  } else {
    splash.poll();
  }
}

void MultiplayerGameHandle::tick(const TimeDiff delta) {
  if (!gameView and conn.skyHandle.getSky()) {
    gameView.emplace(shared, core);
  }

  if (gameView) {
    gameView->tick(delta);
  } else {
    splash.tick(delta);
  }
}

void MultiplayerGameHandle::render(ui::Frame &f) {
  if (!gameView) {
    splash.render(f);
  } else {
    gameView->render(f);
  }
}

bool MultiplayerGameHandle::handle(const sf::Event &event) {
  if (gameView) {
    return gameView->handle(event);
  }
  return false;
}

void MultiplayerGameHandle::reset() {
  if (gameView) gameView->reset();
}

void MultiplayerGameHandle::signalRead() {
  if (gameView) gameView->signalRead();
}

void MultiplayerGameHandle::signalClear() {
  if (gameView) gameView->signalClear();
}

void MultiplayerGameHandle::onChangeSettings(const ui::SettingsDelta &delta) {
  if (gameView) gameView->onChangeSettings(delta);
}

void MultiplayerGameHandle::handleSkyAction(const sky::Action action, const bool state) {
  if (gameView) gameView->handleSkyAction(action, state);
}

void MultiplayerGameHandle::handleClientAction(const ui::ClientAction action, const bool state) {
  if (gameView) gameView->handleClientAction(action, state);
}
