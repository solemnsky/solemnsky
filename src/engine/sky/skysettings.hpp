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
/**
 * Some mutable global settings for a `Sky`. Defaults suggested by a Map.
 */
#pragma once
#include "util/types.hpp"
#include "util/networked.hpp"

namespace sky {

struct SkySettingsInit: public VerifyStructure {
  SkySettingsInit(); // constructs with sensible defaults
  // (Integrated into the Map format.)

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(cereal::make_nvp("view_scale", viewScale),
       cereal::make_nvp("gravity", gravity));
  }

  float viewScale, gravity;

  // VerifyStructure impl.
  bool verifyStructure() const override final;

};

struct SkySettingsDelta: public VerifyStructure {
  SkySettingsDelta();

  template<typename Archive>
  void serialize(Archive &ar) {
    ar(viewScale, gravity);
  }

  optional<float> viewScale, gravity;

  // VerifyStructure impl.
  bool verifyStructure() const override final;

  // Constructors.
  static SkySettingsDelta ChangeGravity(const float gravity);
  static SkySettingsDelta ChangeView(const float view);

};

struct SkySettings: public Networked<SkySettingsInit, SkySettingsDelta> {
 private:
  SkySettingsInit lastSettings;

 public:
  SkySettings() = delete;
  SkySettings(const SkySettingsInit &settings);

  // Values.
  float viewScale, // 1 for normal.
      gravity; // 1 for normal, -1 for inverse.

  // Networked impl.
  void applyDelta(const SkySettingsDelta &delta) override;
  SkySettingsInit captureInitializer() const override;
  SkySettingsDelta collectDelta();

};

}
