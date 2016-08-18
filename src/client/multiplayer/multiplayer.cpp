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
#include "multiplayer.hpp"
#include "client/elements/style.hpp"
#include "multiplayerlobby.hpp"
#include "multiplayergamehandle.hpp"
#include "multiplayerscoring.hpp"

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
      view = std::make_unique<MultiplayerGameHandle>(shared, core);
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
  status = "connecting...";
  areChildComponents({&core});
}

void Multiplayer::onConnect() {
  loadView(core.conn->arena.getMode());
  status = "connected";
}

void Multiplayer::onLoadMode(const sky::ArenaMode mode) {
  if (mode != sky::ArenaMode::Game) loadView(mode);
}

void Multiplayer::onStartGame() {
  if (core.conn->arena.getMode() == sky::ArenaMode::Game) {
    loadView(sky::ArenaMode::Game);
    status = "in game";
  }
}

void Multiplayer::onEndGame() {
  if (core.conn->arena.getMode() == sky::ArenaMode::Game)
    loadView(sky::ArenaMode::Lobby);
  status = "connected";
}

void Multiplayer::onChangeSettings(const ui::SettingsDelta &settings) {
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
    core.conn->debugView.printArenaReport(p);

    p.breakLine();
    p.printTitle("Networking");
    p.printLn("inbound bandwidth (kB/s): "
                  + printKbps(core.getHost().incomingBandwidth()));
    p.printLn("outbound bandwidth (kB/s): "
                  + printKbps(core.getHost().outgoingBandwidth()));
  } else {
    p.printLn("not connected...");
  }
}

void Multiplayer::printDebugRight(Printer &p) {
  if (core.conn) {
    core.conn->debugView.printSkyReport(p);
  } else {
    p.printLn("not connected...");
  }
}

void Multiplayer::poll() {
  core.poll();
}

void Multiplayer::tick(const TimeDiff delta) {
  ui::Control::tick(delta);
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
  if (view) {
    if (view->handle(event)) return true;

    bool bindingFound{false};
    const auto &bindings = settings.bindings;
    for (const auto &action : bindings.findClientActions(event)) {
      bindingFound = true;
      if (core.messageInteraction.handleClientAction(action.first, action.second)) continue;
      view->handleClientAction(action.first, action.second);
    }
    for (const auto &action : bindings.findSkyActions(event)) {
      view->handleSkyAction(action.first, action.second);
      bindingFound = true;
    }

    return bindingFound;
  }

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

std::string Multiplayer::getQuittingReason() const {
  return core.getDisconnectReason();
}


