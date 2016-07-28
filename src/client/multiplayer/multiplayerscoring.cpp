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
#include "multiplayerscoring.hpp"
#include "client/elements/style.hpp"

MultiplayerScoring::MultiplayerScoring(
    ClientShared &shared, MultiplayerCore &connection) :
    MultiplayerView(shared, connection) {
  areChildren({&core.messageInteraction});
}

void MultiplayerScoring::tick(float delta) {

}

void MultiplayerScoring::render(ui::Frame &f) {

}

bool MultiplayerScoring::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void MultiplayerScoring::signalRead() {
  ui::Control::signalRead();

  if (const auto &signal = core.messageInteraction.inputSignal) {
    core.handleChatInput(signal.get());
  }
}

void MultiplayerScoring::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerScoring::onChangeSettings(const ui::SettingsDelta &settings) {

}
