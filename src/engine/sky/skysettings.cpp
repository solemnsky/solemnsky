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
#include "skysettings.hpp"

namespace sky {

/**
 * SkySettingsData.
 */

SkySettingsData::SkySettingsData() :
    viewScale(1), gravity(1) { }

bool SkySettingsData::verifyStructure() const {
  return (viewScale > 0);
}

/**
 * SkySettingsDelta.
 */

SkySettingsDelta::SkySettingsDelta() :
    viewScale(), gravity() { }

bool SkySettingsDelta::verifyStructure() const {
  if (viewScale) return (*viewScale > 0);
  return true;
}

SkySettingsDelta SkySettingsDelta::ChangeGravity(const float gravity) {
  SkySettingsDelta delta;
  delta.gravity = gravity;
  return delta;
}

SkySettingsDelta SkySettingsDelta::ChangeView(const float view) {
  SkySettingsDelta delta;
  delta.viewScale = view;
  return delta;
}

/**
 * SkySettings.
 */

SkySettings::SkySettings(const SkySettingsData &data) :
    AutoNetworked(data),
    data(data),
    modified(false) { }

float SkySettings::getViewscale() const {
  return data.viewScale;
}

float SkySettings::getGravity() const {
  return data.gravity;
}

void SkySettings::applyDelta(const SkySettingsDelta &delta) {
  if (delta.gravity) data.gravity = delta.gravity.get();
  if (delta.viewScale) data.viewScale = delta.viewScale.get();
  modified = true;
}

SkySettingsData SkySettings::captureInitializer() const {
  return data;
}

optional<SkySettingsDelta> SkySettings::collectDelta() {
  if (modified) {
    SkySettingsDelta delta;
    delta.viewScale.emplace(data.viewScale);
    delta.gravity.emplace(data.gravity);
    modified = false;
    return delta;
  }

  return {};
}

}
