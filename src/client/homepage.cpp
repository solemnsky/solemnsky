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
#include "homepage.hpp"
#include "elements/style.hpp"
#include "client/multiplayer/multiplayer.hpp"
#include "client/sandbox/sandbox.hpp"

HomePage::HomePage(ClientShared &clientState) :
    Page(clientState),
    sandboxButton(references, style.base.normalButton,
                  style.home.tutorialButtonPos,
                  "SANDBOX"),
    localhostButton(references, style.base.normalButton,
                    style.home.localhostButtonPos,
                    "LOCAL"),
    remoteButton(references, style.base.normalButton,
                 style.home.remoteButtonPos,
                 "REMOTE"),
    serverEntry(references, style.base.normalTextEntry,
                style.home.serverEntryPos, "server address", true) {
  areChildren({&sandboxButton, &localhostButton, &remoteButton, &serverEntry});

  //Initial value
  serverEntry.contents = "46.101.20.237";
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

  if (sandboxButton.clickSignal)
    shared.beginGame(std::make_unique<Sandbox>(shared));

  if (localhostButton.clickSignal)
    shared.beginGame(
        std::make_unique<Multiplayer>(shared, "localhost", 4242));

  if (remoteButton.clickSignal)
    shared.beginGame(
        std::make_unique<Multiplayer>(shared, serverEntry.contents, 4242));
}

void HomePage::signalClear() {
  ui::Control::signalClear();
}



