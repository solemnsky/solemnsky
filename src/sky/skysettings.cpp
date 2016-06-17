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
 * SkySettingsInit.
 */

SkySettingsInit::SkySettingsInit() :
    viewScale(0.5), gravity(1) {}

bool SkySettingsInit::verifyStructure() const {
  return (viewScale > 0);
}

/**
 * SkySettingsDelta.
 */

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

SkySettings::SkySettings(const SkySettingsInit &settings) :
    Networked(settings),
    lastSettings(settings),
    viewScale(settings.viewScale),
    gravity(settings.gravity) {}

void SkySettings::applyDelta(const SkySettingsDelta &delta) {
  if (delta.gravity) gravity = delta.gravity.get();
  if (delta.viewScale) viewScale = delta.viewScale.get();
}

SkySettingsInit SkySettings::captureInitializer() const {
  SkySettingsInit init;
  init.viewScale = viewScale;
  init.gravity = gravity;
  return init;
}

SkySettingsDelta SkySettings::collectDelta() {
  SkySettingsDelta delta;
  if (lastSettings.viewScale != viewScale) delta.viewScale.emplace(viewScale);
  if (lastSettings.gravity != gravity) delta.gravity.emplace(gravity);
  lastSettings = captureInitializer();
  return delta;
}

}
