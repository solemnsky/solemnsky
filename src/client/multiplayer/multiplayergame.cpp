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
#include "multiplayergame.hpp"
#include "client/elements/style.hpp"

/**
 * MultiplayerGame.
 */

void MultiplayerGame::printScores(ui::TextFrame &tf, const sky::Team team) {
  const sf::Color color = (team == sky::Team::Red) ? sf::Color::Red : sf::Color::Blue;

  tf.setColor(color);
  tf.printLn((team == sky::Team::Red) ? "red team" : "blue team");

  conn.arena.forPlayers([&](const sky::Player &player) {
    if (player.getTeam() == team) {
      tf.setColor(color);
      tf.print(player.getNickname() + " ");

      tf.setColor(style.base.textColor);
      if (player.latencyIsCalculated()) {
        tf.print("ping{" + printTimeDiff(player.getLatency()) + "}");
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
    if (player.getTeam() == sky::Team::Spectator) tf.print(player.getNickname() + " ");
  });
}

void MultiplayerGame::renderScoreboard(ui::Frame &f) {
  f.drawSprite(resources.getTexture(ui::TextureID::ScoreOverlay),
               style.game.scoreboardOffset,
               style.game.scoreboardDisplay);
  f.drawText(
      style.game.scoreboardOffset
          + sf::Vector2f(0, style.game.scoreboardPaddingTop),
      [&](ui::TextFrame &p) { printScores(p, sky::Team::Red); },
      style.base.normalText, resources.defaultFont);

  f.drawText(
      style.game.scoreboardOffset
          + sf::Vector2f(style.game.scoreboardDisplay.width / 2,
                         style.game.scoreboardPaddingTop),
      [&](ui::TextFrame &p) { printScores(p, sky::Team::Blue); },
      style.base.normalText, resources.defaultFont);

  f.drawText(
      style.game.scoreboardOffset +
          sf::Vector2f(0, style.game.scoreboardDisplay.height
              - (2 * style.base.normalFontSize)),
      [&](ui::TextFrame &tf) { printSpectators(tf); },
      style.base.normalText, resources.defaultFont);
}

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerCore &core) :
    MultiplayerView(shared, core),
    scoreboardFocused(false),
    skyRender(shared, resources, conn.arena, *conn.getSky()),
    participation(conn.getSky()->getParticipation(conn.player)) {
  assert(bool(conn.skyHandle.getSky()));
  assert(bool(conn.skyHandle.getEnvironment()));
  assert(bool(conn.skyHandle.getEnvironment()->getVisuals()));

  areChildren({&core.messageInteraction});
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

  ui::Control::render(f);

  if (scoreboardFocused) renderScoreboard(f);
}

bool MultiplayerGame::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void MultiplayerGame::signalRead() {
  ui::Control::signalRead();

  if (const auto &signal = core.messageInteraction.inputSignal) {
    core.handleChatInput(signal.get());
  }
}

void MultiplayerGame::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerGame::handleSkyAction(const sky::Action action, const bool state) {
  participation.doAction(action, state);
}

void MultiplayerGame::handleClientAction(const ui::ClientAction action,
                                         const bool state) {
  switch (action) {
    case ui::ClientAction::Spawn: {
      if (state) {
        if (!participation.isSpawned())
          core.transmit(sky::ClientPacket::ReqSpawn());
      }
      break;
    }
    case ui::ClientAction::Scoreboard: {
      scoreboardFocused = state;
      break;
    }
    default: {
    }
  }
}

/**
 * MultiplayerSplash.
 */

MultiplayerSplash::MultiplayerSplash(ClientShared &shared, MultiplayerCore &core) :
    MultiplayerView(shared, core) { }

void MultiplayerSplash::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerSplash::render(ui::Frame &f) {
  if (conn.skyHandle.getEnvironment()->loadingErrored()) {
    f.drawText({500, 500}, "Environment loading errored!", sf::Color::White, style.base.normalText,
               resources.defaultFont);
  } else {
    f.drawText({500, 500}, "loading environment...", sf::Color::White, style.base.normalText,
               resources.defaultFont);
  }
}

bool MultiplayerSplash::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void MultiplayerSplash::signalRead() {
  ui::Control::signalRead();
}

void MultiplayerSplash::signalClear() {
  ui::Control::signalClear();
}
