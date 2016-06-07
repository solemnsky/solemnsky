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
#include "multiplayergame.h"
#include "client/elements/style.h"

void MultiplayerGame::doClientAction(const ClientAction action,
                                     const bool state) {
  switch (action) {
    case ClientAction::Spawn: {
      if (state) core.transmit(sky::ClientPacket::ReqSpawn());
      break;
    }
    case ClientAction::Chat: {
      if (state) chatInput.focus();
      break;
    }
    case ClientAction::Scoreboard: {
      scoreboardFocused = state;
      break;
    }
  }
}

void MultiplayerGame::printScores(ui::TextFrame &tf, const sky::Team team) {
  const sf::Color color = (team == 1) ? sf::Color::Red : sf::Color::Blue;

  tf.setColor(color);
  tf.printLn((team == 1) ? "red team" : "blue team");

  conn.arena.forPlayers([&](const sky::Player &player) {
    if (player.getTeam() == team) {
      tf.setColor(color);
      tf.print(player.getNickname() + " ");

      tf.setColor(style.base.textColor);
      if (player.latencyIsCalculated()) {
        tf.print("ping{" + showTimeDiff(player.getLatency()) + "}");
      } else {
        tf.print("ping{unknown}");
      }

      tf.breakLine();
    }
  });
}

void MultiplayerGame::printSpectators(ui::TextFrame &tf) {
  tf.setColor(style.base.textColor);
  tf.print("spectators: ");
  conn.arena.forPlayers([&](const sky::Player &player) {
    if (player.getTeam() == 0) tf.print(player.getNickname() + " ");
  });
}

void MultiplayerGame::renderScoreboard(ui::Frame &f) {
  f.drawSprite(resources.getTexture(ui::TextureID::ScoreOverlay),
               style.multi.scoreboardOffset,
               style.multi.scoreboardDisplay);
  f.drawText(
      style.multi.scoreboardOffset
          + sf::Vector2f(0, style.multi.scoreboardPaddingTop),
      [&](ui::TextFrame &p) { printScores(p, 1); },
      style.base.normalText, resources.defaultFont);

  f.drawText(
      style.multi.scoreboardOffset
          + sf::Vector2f(style.multi.scoreboardDisplay.width / 2,
                         style.multi.scoreboardPaddingTop),
      [&](ui::TextFrame &p) { printScores(p, 2); },
      style.base.normalText, resources.defaultFont);

  f.drawText(
      style.multi.scoreboardOffset +
          sf::Vector2f(0, style.multi.scoreboardDisplay.height
              - (2 * style.base.normalFontSize)),
      [&](ui::TextFrame &tf) { printSpectators(tf); },
      style.base.normalText, resources.defaultFont);
}

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerCore &connection) :
    MultiplayerView(sky::ArenaMode::Game, shared, connection),
    chatInput(appState,
              style.base.normalTextEntry,
              style.multi.chatPos,
              "[ENTER TO CHAT]"),
    scoreboardFocused(false),
    skyRender(shared, conn.arena, conn.getSky().get()),
    participation(conn.getSky()->getParticipation(conn.player)) {
  assert(conn.skyHandle.isActive());
  areChildren({&chatInput});
  areChildComponents({&skyRender});
}

void MultiplayerGame::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerGame::render(ui::Frame &f) {
  skyRender.render(
      f, participation.plane ?
         participation.plane->getState().physical.pos :
         sf::Vector2f(0, 0));

  f.drawText(style.multi.chatPos, [&](ui::TextFrame &tf) {
    if (chatInput.isFocused) core.drawEventLog(tf, style.multi.chatCutoff);
    else core.drawEventLog(tf, style.multi.chatIngameCutoff);
  }, style.multi.messageLogText, defaultFont);

  ui::Control::render(f);

  if (scoreboardFocused) renderScoreboard(f);
}

bool MultiplayerGame::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  if (participation.isSpawned()) {
    if (auto action = shared.triggerSkyAction(event)) {
      conn.player.doAction(action->first, action->second);
      return true;
    }
  }

  if (auto clientAction = shared.triggerClientAction(event)) {
    doClientAction(clientAction->first, clientAction->second);
    return true;
  }

  return false;
}

void MultiplayerGame::signalRead() {
  ui::Control::signalRead();
  if (chatInput.inputSignal) {
    core.handleChatInput(chatInput.inputSignal.get());
  }
}

void MultiplayerGame::signalClear() {
  ui::Control::signalClear();
}
