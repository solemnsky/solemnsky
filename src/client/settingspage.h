/**
 * Settings page.
 */
#pragma once
#include "elements/elements.h"
#include "ui/widgets/widgets.h"

/**
 * One tab of the control page.
 * Simple user interface that can load / write from and to a Settings object.
 */
class SettingsTab: public ui::Control {
 public:
  SettingsTab(ui::AppState &appState, const Settings &settings) :
      ui::Control(appState) { }

  virtual void readSettings(const Settings &settings) = 0;
  virtual void writeSettings(Settings &settings) = 0;
};

class GeneralTab: public SettingsTab {
 private:
  ui::Checkbox debugOption;

 public:
  GeneralTab(const Settings &settings);

  void readSettings(const Settings &settings) override final;
  void writeSettings(Settings &buffer) override final;
};

class PlayerTab: public SettingsTab {
 private:
  ui::TextEntry nicknameOption;

 public:
  PlayerTab(const Settings &settings);

  void readSettings(const Settings &settings) override final;
  void writeSettings(Settings &settings) override final;
};

class ControlsTab: public SettingsTab {
 private:
  std::vector<sky::Action> skyActions;
  std::vector<ClientAction> clientActions;
  std::map<sky::Action, ui::KeySelector> skyChoosers;
  std::map<ClientAction, ui::KeySelector> clientChoosers;

 public:
  ControlsTab(const Settings &settings);

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

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  void onChangeSettings(const SettingsDelta &) override;
  void onBlur() override;
};
