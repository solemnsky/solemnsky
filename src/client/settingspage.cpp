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
#include "settingspage.hpp"
#include "elements/style.hpp"

/**
 * SettingsTab.
 */

SettingsTab::SettingsTab(const ui::AppRefs &references) :
    ui::Control(references) {}

/**
 * GeneralTab.
 */

GeneralTab::GeneralTab(
    const ui::AppRefs &references) :
    SettingsTab(references),
    debugOption(references,
                style.settings.checkbox,
                style.settings.column1Pos),
    fullscreenOption(references, style.settings.checkbox,
                     style.settings.column1Pos + sf::Vector2f(0, style.settings.rowOffset)) {
  debugOption.setDescription(optional<std::string>("debug mode"));
  fullscreenOption.setDescription(optional<std::string>("full-screen"));

  areChildren({&debugOption, &fullscreenOption});
  readSettings(settings);
}

void GeneralTab::readSettings(const ui::Settings &settings) {
  debugOption.setValue(settings.enableDebug);
  fullscreenOption.setValue(settings.fullscreen);
}

void GeneralTab::writeSettings(ui::Settings &settings) const {
  settings.enableDebug = debugOption.getValue();
  settings.fullscreen = fullscreenOption.getValue();
}

/**
 * PlayerTab.
 */

PlayerTab::PlayerTab(
    const ui::AppRefs &references) :
    SettingsTab(references),
    nicknameOption(references, style.base.normalTextEntry,
                   style.settings.column1Pos) {
  nicknameOption.persistent = true;
  nicknameOption.description = "nickname";
  areChildren({&nicknameOption});
  readSettings(settings);
}

void PlayerTab::readSettings(const ui::Settings &settings) {
  nicknameOption.contents = settings.nickname;
}

void PlayerTab::writeSettings(ui::Settings &settings) const {
  settings.nickname = nicknameOption.contents;
}

/**
 * ControlsTab.
 */

ControlsTab::ControlsTab(
    const ui::AppRefs &references) :
    SettingsTab(references) {
  sf::Vector2f pos = style.settings.column1Pos;

  sky::forSkyActions([&](const sky::Action action) {
    skyBindingChoosers.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(action),
        std::forward_as_tuple(references, style.base.normalButton, pos));

    auto &selector = skyBindingChoosers.at(action);
    selector.setDescription(sky::showAction(action));
    areChildren({&selector});

    pos += {0, style.settings.rowOffset};
  });

  pos = style.settings.column2Pos;

  ui::forClientActions([&](const ui::ClientAction action) {
    clientBindingChoosers.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(action),
        std::forward_as_tuple(references, style.base.normalButton, pos));

    auto &selector = clientBindingChoosers.at(action);
    selector.setDescription(showClientAction(action));
    areChildren({&selector});

    pos += {0, style.settings.rowOffset};
  });

  readSettings(settings);
}

void ControlsTab::readSettings(const ui::Settings &settings) {
  sky::forSkyActions([&](const sky::Action action) {
    skyBindingChoosers.at(action).setValue(
        settings.bindings.lookupBinding(action));
  });

  ui::forClientActions([&](const ui::ClientAction action) {
    clientBindingChoosers.at(action).setValue(
        settings.bindings.lookupClientBinding(action));
  });
}

void ControlsTab::writeSettings(ui::Settings &settings) const {
  settings.bindings.skyBindings.clear();

  sky::forSkyActions([&](const sky::Action action) {
    if (const auto binding = skyBindingChoosers.at(action).getValue()) {
      settings.bindings.skyBindings.emplace(binding.get(), action);
    }
  });

  settings.bindings.clientBindings.clear();

  ui::forClientActions([&](const ui::ClientAction action) {
    if (const auto binding = clientBindingChoosers.at(action).getValue()) {
      settings.bindings.clientBindings.emplace(binding.get(), action);
    }
  });
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

    newSettings(settings),

    generalButton(references, style.settings.sectionButton,
                  {style.settings.generalButtonOffset,
                   style.settings.pageButtonHeight},
                  "GENERAL"),
    playerButton(references, style.settings.sectionButton,
                 {style.settings.playerButtonOffset,
                  style.settings.pageButtonHeight},
                 "PLAYER"),
    controlsButton(references, style.settings.sectionButton,
                   {style.settings.controlsButtonOffset,
                    style.settings.pageButtonHeight},
                   "CONTROLS"),

    generalTab(references),
    playerTab(references),
    controlsTab(references),
    currentTab(&generalButton, &generalTab) {
  areChildren(
      {&generalButton, &playerButton, &controlsButton});
  currentTab.first->setActive(false);
}

SettingsPage::~SettingsPage() {
  onBlur();
}

void SettingsPage::onChangeSettings(const ui::SettingsDelta &delta) {
  newSettings = settings;
  currentTab.second->readSettings(settings);
}

void SettingsPage::onFocus() {
  newSettings = settings;
  currentTab.second->readSettings(settings);
}

void SettingsPage::onBlur() {
  currentTab.second->reset();
  currentTab.second->writeSettings(newSettings);
  shared.changeSettings(ui::SettingsDelta(settings, newSettings));
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
