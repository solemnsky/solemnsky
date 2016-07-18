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
#include "clientui.hpp"
#include "style.hpp"

MessageInteraction::MessageInteraction() :
    Control(references),
    messageEntry(references,
                 style.base.normalTextEntry,
                 style.game.chatPos,
                 "[ENTER TO CHAT]") {}
//    messageLog(references) {}

void MessageInteraction::tick(const TimeDiff delta) {
  ui::Control::tick(delta);
}

void MessageInteraction::render(ui::Frame &f) {
  ui::Control::render(f);
}

bool MessageInteraction::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void MessageInteraction::reset() {
  ui::Control::reset();
}

void MessageInteraction::signalRead() {
  ui::Control::signalRead();
}

void MessageInteraction::signalClear() {
  ui::Control::signalClear();
}
