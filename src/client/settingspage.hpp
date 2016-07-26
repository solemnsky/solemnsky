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
#include "ui/widgets.hpp"

/**
 * One tab of the control page.
 * A Control that can read and write to a settings object.
 */
class SettingsTab : public ui::Control {
 public:
  SettingsTab(const ui::AppRefs &references);

  virtual void readSettings(const ui::Settings &settings) = 0;
  virtual void writeSettings(ui::Settings &settings) const = 0;

};

class GeneralTab : public SettingsTab {
 private:
  ui::Checkbox debugOption;

 public:
  GeneralTab(const ui::AppRefs &references);

  void readSettings(const ui::Settings &settings) override final;
  void writeSettings(ui::Settings &settings) const override final;

};

class PlayerTab : public SettingsTab {
 private:
  ui::TextEntry nicknameOption;

 public:
  PlayerTab(const ui::AppRefs &references, const ui::Settings &settings);

  void readSettings(const ui::Settings &settings) override final;
  void writeSettings(ui::Settings &settings) override final;

};

class ControlsTab : public SettingsTab {
 private:
  std::map<sky::Action, ui::KeySelector> skyBindingChoosers;
  std::map<ui::ClientAction, ui::KeySelector> clientBindingChoosers;

 public:
  ControlsTab(const ui::AppRefs &references, const ui::Settings &settings);

  void readSettings(const ui::Settings &settings) override final;
  void writeSettings(ui::Settings &buffer) const override final;

};

class SettingsPage : public Page {
 private:
  ui::Settings newSettings;

  ui::Button generalButton, playerButton, controlsButton;

  GeneralTab generalTab;
  PlayerTab playerTab;
  ControlsTab controlsTab;

  std::pair<ui::Button *, SettingsTab *> currentTab;

  void switchToTab(ui::Button &button, SettingsTab &tab);

 public:
  SettingsPage(ClientShared &shared);
  ~SettingsPage();

  // Page impl.
  void onChangeSettings(const ui::SettingsDelta &) override;
  void onFocus() override;
  void onBlur() override;

  // Control impl.
  void tick(float delta) override final;
  void render(ui::Frame &f) override final;
  bool handle(const sf::Event &event) override final;
  void reset() override final;
  void signalRead() override final;
  void signalClear() override final;
};
