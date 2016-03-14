#include "settings.h"
#include <cereal/archives/xml.hpp>
#include <fstream>

/**
 * Settings.
 */

template<typename Archive>
void serialize(Archive &ar, Settings &settings) {
  ar(cereal::make_nvp("enableDebug", settings.enableDebug),
     cereal::make_nvp("nickname", settings.nickname));
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
