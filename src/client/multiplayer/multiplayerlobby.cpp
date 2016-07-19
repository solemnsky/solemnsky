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
#include "multiplayerlobby.hpp"
#include "client/elements/style.hpp"

void MultiplayerLobby::doClientAction(const ClientAction action,
                                      const bool state) {
  if (core.messageInteraction.handleClientAction(action, state)) return;

  switch (action) {
    case ClientAction::Spawn:
      break;
    case ClientAction::Scoreboard:
      break;
    default:
      throw enum_error();
  }
}

MultiplayerLobby::MultiplayerLobby(
    ClientShared &shared, MultiplayerCore &connection) :
    MultiplayerView(shared, connection),

    specButton(references, style.base.normalButton, style.game.lobbyButtonPos,
               "SPECTATE"),
    redButton(references, style.base.normalButton,
              style.game.lobbyButtonPos + style.game.lobbyButtonSep,
              "JOIN RED"),
    blueButton(references, style.base.normalButton,
               style.game.lobbyButtonPos + 2.0f * style.game.lobbyButtonSep,
               "JOIN BLUE") {
  areChildren({&specButton, &redButton, &blueButton, &core.messageInteraction});
}

void MultiplayerLobby::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerLobby::render(ui::Frame &f) {
  f.drawSprite(resources.getTexture(ui::TextureID::Lobby),
               {0, 0}, {0, 0, 1600, 900});

  f.drawText(
      style.game.playerListPos, [&](Printer &p) {
        conn.arena.forPlayers([&](const sky::Player &player) {
          p.setColor(0, 0, 0);
          if (player.getTeam() == sky::Team::Red) p.setColor(255, 0, 0);
          if (player.getTeam() == sky::Team::Blue) p.setColor(0, 0, 255);
          p.print(player.getNickname());
          p.breakLine();
        });
      }, style.game.playerListText, resources.defaultFont);

  ui::Control::render(f);
}

bool MultiplayerLobby::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  if (const auto action = shared.triggerClientAction(event)) {
    doClientAction(action->first, action->second);
    return true;
  }

  return false;
}

void MultiplayerLobby::reset() {
  ui::Control::reset();
}

void MultiplayerLobby::signalRead() {
  ui::Control::signalRead();

  if (specButton.clickSignal)
    core.requestTeamChange(sky::Team::Spectator);
  if (redButton.clickSignal)
    core.requestTeamChange(sky::Team::Red);
  if (blueButton.clickSignal)
    core.requestTeamChange(sky::Team::Blue);
  if (const auto &signal = core.messageInteraction.inputSignal) {
    core.handleChatInput(signal.get());
  }
}

void MultiplayerLobby::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerLobby::onChangeSettings(const SettingsDelta &settings) {

}

