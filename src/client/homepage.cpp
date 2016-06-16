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
#include "homepage.h"
#include "util/methods.h"
#include "elements/style.h"
#include "client/multiplayer/multiplayer.h"
#include "client/tutorial/tutorial.h"

HomePage::HomePage(ClientShared &clientState) :
    Page(clientState),
    tutorialButton(references, style.base.normalButton,
                   style.home.tutorialButtonPos,
                   "TUTORIAL"),
    localhostButton(references, style.base.normalButton,
                    style.home.localhostButtonPos,
                    "LOCAL GAME"),
    remoteButton(references, style.base.normalButton,
                 style.home.remoteButtonPos,
                 "REMOTE GAME") {
  areChildren({&tutorialButton, &localhostButton, &remoteButton});
}

void HomePage::tick(float delta) {
  ui::Control::tick(delta);
}

void HomePage::onChangeSettings(const SettingsDelta &settings) {

}

void HomePage::onBlur() {

}

void HomePage::render(ui::Frame &f) {
  drawBackground(f);
  ui::Control::render(f);
}

bool HomePage::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void HomePage::reset() {
  ui::Control::reset();
}

void HomePage::signalRead() {
  ui::Control::signalRead();

  if (tutorialButton.clickSignal)
    shared.beginGame(std::make_unique<Sandbox>(shared));

  if (localhostButton.clickSignal)
    shared.beginGame(
        std::make_unique<Multiplayer>(shared, "localhost", 4242));

  if (remoteButton.clickSignal)
    shared.beginGame(
        std::make_unique<Multiplayer>(shared, "46.101.20.237", 4242));
}

void HomePage::signalClear() {
  ui::Control::signalClear();
}



