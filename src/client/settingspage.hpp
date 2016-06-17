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
 * Settings page.
 */
#pragma once
#include "elements/elements.hpp"
#include "ui/widgets/widgets.hpp"

/**
 * One tab of the control page.
 * Simple user interface that can load / write from and to a Settings object.
 */
class SettingsTab: public ui::Control {
 public:
  SettingsTab(const ui::AppRefs &references, const Settings &settings);

  virtual void readSettings(const Settings &settings) = 0;
  virtual void writeSettings(Settings &settings) = 0;
};

class GeneralTab: public SettingsTab {
 private:
  ui::Checkbox debugOption;

 public:
  GeneralTab(const ui::AppRefs &references, const Settings &settings);

  void readSettings(const Settings &settings) override final;
  void writeSettings(Settings &buffer) override final;
};

class PlayerTab: public SettingsTab {
 private:
  ui::TextEntry nicknameOption;

 public:
  PlayerTab(const ui::AppRefs &references, const Settings &settings);

  void readSettings(const Settings &settings) override final;
  void writeSettings(Settings &settings) override final;
};

class ControlsTab: public SettingsTab {
 private:
  std::vector<sky::Action> skyActions;
  std::vector<ClientAction> clientActions;
  std::map<sky::Action, ui::KeySelector> skyBindingChoosers;
  std::map<ClientAction, ui::KeySelector> clientBindingChoosers;

 public:
  ControlsTab(const ui::AppRefs &references, const Settings &settings);

  void readSettings(const Settings &settings) override final;
  void writeSettings(Settings &buffer) override final;
};

class SettingsPage: public Page {
 private:
  Settings newSettings;

  ui::Button generalButton, playerButton, controlsButton;

  GeneralTab generalTab;
  PlayerTab playerTab;
  ControlsTab controlsTab;

  std::pair<ui::Button *, SettingsTab *> currentTab;

  void switchToTab(ui::Button &button, SettingsTab &tab);

 public:
  SettingsPage(ClientShared &shared);

  // Page impl.
  void onChangeSettings(const SettingsDelta &) override;
  void onBlur() override;

  // Control impl.
  void tick(float delta) override final;
  void render(ui::Frame &f) override final;
  bool handle(const sf::Event &event) override final;
  void reset() override final;
  void signalRead() override final;
  void signalClear() override final;
};
