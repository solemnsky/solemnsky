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
#include <cereal/archives/xml.hpp>
#include <fstream>
#include "settings.hpp"
#include "util/methods.hpp"

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
     cereal::make_nvp("nickname", settings.nickname),
     cereal::make_nvp("bindings", settings.bindings));
}

template<typename Archive>
void serialize(Archive &ar, KeyBindings &bindings) {
  ar(cereal::make_nvp("engine", bindings.skyBindings),
     cereal::make_nvp("client", bindings.clientBindings));
}

/**
 * Settings.
 */

KeyBindings::KeyBindings() {
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

optional<sf::Keyboard::Key> KeyBindings::lookupBinding(
    const sky::Action action) const {
  for (const auto pair : skyBindings)
    if (pair.second == action) return {pair.first};
  return {};
}

optional<sf::Keyboard::Key> KeyBindings::lookupClientBinding(
    const ClientAction action) const {
  for (const auto pair : clientBindings)
    if (pair.second == action) return {pair.first};
  return {};
}

Settings::Settings() :
    enableDebug(false),
    nickname("nameless plane") { }

void Settings::readFromFile(const std::string &filepath) {
  std::ifstream file(filepath);
  cereal::XMLInputArchive archive(file);
  archive(*this);
}

void Settings::writeToFile(const std::string &filepath) {
  std::ofstream file(filepath);
  cereal::XMLOutputArchive archive(file);
  archive(*this);
}

/**
 * SettingsDelta.
 */

SettingsDelta::SettingsDelta() { }

SettingsDelta::SettingsDelta(const Settings &oldSettings,
                             const Settings &newSettings) {
  if (oldSettings.nickname != newSettings.nickname)
    nickname = newSettings.nickname;
  if (oldSettings.enableDebug != newSettings.enableDebug)
    enableDebug = newSettings.enableDebug;
  bindings = newSettings.bindings;
}

void SettingsDelta::apply(Settings &settings) const {
  if (nickname) settings.nickname = *nickname;
  if (enableDebug) settings.enableDebug = *enableDebug;
  if (bindings) settings.bindings = *bindings;
}

