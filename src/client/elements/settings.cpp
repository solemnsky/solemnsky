#include "settings.h"
#include <cereal/archives/xml.hpp>
#include <fstream>

/**
 * Settings.
 */

KeyBindings::KeyBindings() :
    thrust(sf::Keyboard::I),
    reverse(sf::Keyboard::K),
    left(sf::Keyboard::J),
    right(sf::Keyboard::L),
    primary(sf::Keyboard::F),
    secondary(sf::Keyboard::D),
    special(sf::Keyboard::S),
    chat(sf::Keyboard::Return),
    scoreboard(sf::Keyboard::Tab) { }

template<typename Archive>
void serialize(Archive &ar, const sf::Keyboard::Key key) {
  ar((unsigned char) key);
}

template<typename Archive>
void serialize(Archive &ar, const KeyBindings &bindings) {
  ar(cereal::make_nvp("thrust", bindings.thrust),
     cereal::make_nvp("reverse", bindings.reverse),
     cereal::make_nvp("left", bindings.left),
     cereal::make_nvp("right", bindings.right),
     cereal::make_nvp("primary", bindings.primary),
     cereal::make_nvp("secondary", bindings.secondary),
     cereal::make_nvp("special", bindings.special),
     cereal::make_nvp("chat", bindings.chat),
     cereal::make_nvp("scoreboard", bindings.scoreboard));
}

template<typename Archive>
void serialize(Archive &ar, Settings &settings) {
  ar(cereal::make_nvp("enableDebug", settings.enableDebug),
     cereal::make_nvp("nickname", settings.nickname),
     cereal::make_nvp("bindings", settings.bindings));
}

Settings::Settings() :
    nickname("nameless plane"),
    enableDebug(false) { }

void Settings::readFromFile(std::string filepath) {
  std::ifstream file(filepath);
  cereal::XMLInputArchive archive(file);
  archive(*this);
}

void Settings::writeToFile(std::string filepath) {
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
}

void SettingsDelta::apply(Settings &settings) const {
  if (nickname) settings.nickname = *nickname;
  if (enableDebug) settings.enableDebug = *enableDebug;
}

