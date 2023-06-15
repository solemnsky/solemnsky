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
#include <fstream>
#include "settings.hpp"
#include "util/methods.hpp"

namespace ui {

void forClientActions(std::function<void(const ClientAction)> fn) {
  for (ClientAction action = ClientAction(0);
       action < ClientAction::MAX;
       action = ClientAction(size_t(action) + 1)) {
    fn(action);
  }
}

std::string showClientAction(const ClientAction action) {
  switch (action) {
    case ClientAction::Spawn:
      return "spawn";
    case ClientAction::Chat:
      return "chat";
    case ClientAction::Scoreboard:
      return "scoreboard";
    default:
      throw enum_error();
  }
}

/**
 * Serialization through cereal.
 */

template<typename Archive>
void serialize(Archive &ar, Settings &settings) {
  ar(cereal::make_nvp("enableDebug", settings.enableDebug),
     cereal::make_nvp("fullscreen", settings.fullscreen),
     cereal::make_nvp("resolution", settings.resolution),
     cereal::make_nvp("nickname", settings.nickname),
     cereal::make_nvp("bindings", settings.bindings));
}

template<typename Archive>
void serialize(Archive &ar, ActionBindings &bindings) {
  ar(cereal::make_nvp("engine", bindings.skyBindings),
     cereal::make_nvp("client", bindings.clientBindings));
}

/**
 * Settings.
 */

ActionBindings::ActionBindings() {
  skyBindings.emplace(sf::Keyboard::I, sky::Action::Thrust);
  skyBindings.emplace(sf::Keyboard::K, sky::Action::Reverse);
  skyBindings.emplace(sf::Keyboard::J, sky::Action::Left);
  skyBindings.emplace(sf::Keyboard::L, sky::Action::Right);
  skyBindings.emplace(sf::Keyboard::F, sky::Action::Primary);
  skyBindings.emplace(sf::Keyboard::D, sky::Action::Secondary);
  skyBindings.emplace(sf::Keyboard::S, sky::Action::Special);
  skyBindings.emplace(sf::Keyboard::Q, sky::Action::Suicide);

  clientBindings.emplace(sf::Keyboard::F, ClientAction::Spawn);
  clientBindings.emplace(sf::Keyboard::Return, ClientAction::Chat);
  clientBindings.emplace(sf::Keyboard::Tab, ClientAction::Scoreboard);
}

optional<InputAction> ActionBindings::lookupBinding(
    const sky::Action action) const {
  for (const auto &pair : skyBindings)
    if (pair.second == action) return {pair.first};
  return {};
}

optional<InputAction> ActionBindings::lookupClientBinding(
    const ClientAction action) const {
  for (const auto &pair : clientBindings)
    if (pair.second == action) return {pair.first};
  return {};
}

template<typename T>
std::vector<std::pair<T, bool>>
bindingsFromEvent(const sf::Event &event,
                  const std::map<ui::InputAction, T> &map) {
  const auto actions = ui::InputAction::actionsForEvent(event);
  std::vector<std::pair<T, bool>> makes;

  for (const ui::InputAction &action : actions) {
    bool make = action.isMake(event);
    const auto &find = map.find(action);

    if (find != map.end()) {
      makes.push_back(std::make_pair(find->second, make));
    }
  }

  return makes;
}

std::vector<std::pair<sky::Action, bool>>
ActionBindings::findSkyActions(const sf::Event &event) const {
  return bindingsFromEvent(event, skyBindings);
}

std::vector<std::pair<ui::ClientAction, bool>>
ActionBindings::findClientActions(const sf::Event &event) const {
  return bindingsFromEvent(event, clientBindings);
}

/**
 * Settings.
 */

Settings::Settings(const std::string &filepath) :
    fullscreen(false),
    resolution(1600, 900),
    enableDebug(false),
    nickname("nameless plane") {
  appLog("Loading client settings from " + inQuotes(filepath), LogOrigin::Client);

  std::ifstream file(filepath);
  if (file.is_open()) {
    cereal::JSONInputArchive archive(file);
    try {
      archive(*this);
      appLog("Settings loaded!",
             LogOrigin::Client);
      return;
    } catch (cereal::Exception &) {
      appLog("File appears invalid. Was it written by an earlier version of the client?",
             LogOrigin::Client);
    }
  } else {
    appLog("File does not exist.",
           LogOrigin::Client);
  }

  appLog("Using default settings.", LogOrigin::Client);
}

void Settings::writeToFile(const std::string &filepath) const {
  appLog("Writing client settings to " + inQuotes(filepath), LogOrigin::Client);

  std::ofstream file(filepath);
  if (file.is_open()) {
    cereal::JSONOutputArchive archive(file);
    archive(*this);
  } else {
    appLog("Could not open settings file for writing!", LogOrigin::Error);
  }
}

/**
 * SettingsDelta.
 */

SettingsDelta::SettingsDelta() { }

SettingsDelta::SettingsDelta(const Settings &oldSettings,
                             const Settings &newSettings) {
  if (oldSettings.fullscreen != newSettings.fullscreen)
    fullscreen = newSettings.fullscreen;
  if (oldSettings.nickname != newSettings.nickname)
    nickname = newSettings.nickname;
  if (oldSettings.enableDebug != newSettings.enableDebug)
    enableDebug = newSettings.enableDebug;
  bindings = newSettings.bindings;
}

void SettingsDelta::apply(Settings &settings) const {
  if (fullscreen) settings.fullscreen = *fullscreen;
  if (nickname) settings.nickname = *nickname;
  if (enableDebug) settings.enableDebug = *enableDebug;
  if (bindings) settings.bindings = *bindings;
}

}
