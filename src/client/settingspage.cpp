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
#include "settingspage.h"
#include "elements/style.h"
#include "util/methods.h"

/**
 * GeneralTab.
 */

GeneralTab::GeneralTab(ui::AppState &appState, const Settings &settings) :
    SettingsTab(appState, settings),
    debugOption(appState, style.settings.checkbox, style.settings.column1Pos) {
  debugOption.setDescription(optional<std::string>("debug mode"));
  areChildren({&debugOption});
  readSettings(settings);
}

void GeneralTab::readSettings(const Settings &buffer) {
  debugOption.setValue(buffer.enableDebug);
}

void GeneralTab::writeSettings(Settings &buffer) {
  buffer.enableDebug = debugOption.getValue();
}

/**
 * PlayerTab.
 */

PlayerTab::PlayerTab(ui::AppState &appState, const Settings &settings) :
    SettingsTab(appState, settings),
    nicknameOption(appState, style.base.normalTextEntry,
                   style.settings.column1Pos) {
  nicknameOption.persistent = true;
  nicknameOption.description = "nickname";
  areChildren({&nicknameOption});
  readSettings(settings);
}

void PlayerTab::readSettings(const Settings &settings) {
  nicknameOption.contents = settings.nickname;
}

void PlayerTab::writeSettings(Settings &settings) {
  settings.nickname = nicknameOption.contents;
}

/**
 * ControlsTab.
 */

ControlsTab::ControlsTab(ui::AppState &appState, const Settings &settings) :
    SettingsTab(appState, settings),
    skyActions{sky::Action::Left,
               sky::Action::Right,
               sky::Action::Thrust,
               sky::Action::Reverse,
               sky::Action::Primary,
               sky::Action::Secondary,
               sky::Action::Special,
               sky::Action::Suicide},
    clientActions{
        ClientAction::Spawn,
        ClientAction::Chat,
        ClientAction::Scoreboard
    } {

  sf::Vector2f pos = style.settings.column1Pos;

  for (sky::Action action : skyActions) {
    skyChoosers.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(action),
        std::forward_as_tuple(appState, style.base.normalButton, pos));

    auto &selector = skyChoosers.at(action);
    selector.setDescription(sky::showAction(action));
    areChildren({&selector});

    pos += {0, style.settings.rowOffset};
  }

  pos = style.settings.column2Pos;

  for (ClientAction action : clientActions) {
    clientChoosers.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(action),
        std::forward_as_tuple(appState, style.base.normalButton, pos));

    auto &selector = clientChoosers.at(action);
    selector.setDescription(showClientAction(action));
    areChildren({&selector});

    pos += {0, style.settings.rowOffset};
  }

  readSettings(settings);
}

void ControlsTab::readSettings(const Settings &settings) {
  for (const sky::Action action : skyActions) {
    skyChoosers.at(action).setValue(
        settings.bindings.lookupBinding(action));
  }

  for (const ClientAction action : clientActions) {
    clientChoosers.at(action).setValue(
        settings.bindings.lookupClientBinding(action));
  }
}

void ControlsTab::writeSettings(Settings &settings) {
  settings.bindings.skyBindings.clear();
  for (const sky::Action action : skyActions) {
    if (const auto binding = skyChoosers.at(action).getValue()) {
      settings.bindings.skyBindings.emplace(binding.get(), action);
    }
  }
  settings.bindings.clientBindings.clear();
  for (const ClientAction action : clientActions) {
    if (const auto binding = clientChoosers.at(action).getValue()) {
      settings.bindings.clientBindings.emplace(binding.get(), action);
    }
  }
}

/**
 * SettingsPage.
 */

void SettingsPage::switchToTab(ui::Button &button, SettingsTab &tab) {
  currentTab.first->setActive(true);
  currentTab.second->reset();
  currentTab.second->writeSettings(newSettings);

  button.setActive(false);
  tab.readSettings(newSettings);
  currentTab = std::make_pair(&button, &tab);
}

SettingsPage::SettingsPage(ClientShared &shared) :
    Page(shared),

    newSettings(shared.settings),

    generalButton(appState, style.base.normalButton,
                  {style.settings.generalButtonOffset,
                   style.settings.pageButtonHeight},
                  "GENERAL"),
    playerButton(appState, style.base.normalButton,
                 {style.settings.playerButtonOffset,
                  style.settings.pageButtonHeight},
                 "PLAYER"),
    controlsButton(appState, style.base.normalButton,
                   {style.settings.controlsButtonOffset,
                    style.settings.pageButtonHeight},
                   "CONTROLS"),

    generalTab(appState, shared.settings),
    playerTab(appState, shared.settings),
    controlsTab(appState, shared.settings),
    currentTab(&generalButton, &generalTab) {
  areChildren(
      {&generalButton, &playerButton, &controlsButton});
  currentTab.first->setActive(false);
}

void SettingsPage::tick(float delta) {
  currentTab.second->tick(delta);
  ui::Control::tick(delta);
}

void SettingsPage::render(ui::Frame &f) {
  drawBackground(f);
  currentTab.second->render(f);
  ui::Control::render(f);
}

bool SettingsPage::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;
  return currentTab.second->handle(event);
}

void SettingsPage::reset() {
  ui::Control::reset();
}

void SettingsPage::signalRead() {
  ui::Control::signalRead();
  currentTab.second->signalRead();
  if (generalButton.clickSignal) switchToTab(generalButton, generalTab);
  if (playerButton.clickSignal) switchToTab(playerButton, playerTab);
  if (controlsButton.clickSignal) switchToTab(controlsButton, controlsTab);
}

void SettingsPage::signalClear() {
  ui::Control::signalClear();
  currentTab.second->signalClear();
}

void SettingsPage::onBlur() {
  currentTab.second->reset();
  currentTab.second->writeSettings(newSettings);
  shared.changeSettings(SettingsDelta(shared.settings, newSettings));
}

void SettingsPage::onChangeSettings(const SettingsDelta &delta) {
  newSettings = shared.settings;
  currentTab.second->readSettings(shared.settings);
}

