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
  SettingsTab() = default;

  virtual void readFromBuffer(Settings &buffer) = 0;
  virtual void writeToBuffer(Settings &buffer) = 0;
};

class GeneralTab: public SettingsTab {
 private:
  ui::Checkbox debugOption;
 public:
  GeneralTab(Settings &newSettings, ClientShared &state);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  void update(const SettingsDelta &delta);
  void applyChanges() const;
};

class PlayerTab: public SettingsTab {
 private:
  ui::TextEntry nicknameOption;
 public:
  PlayerTab(ui::Button *const selectorButtom,
            Settings &newSettings, ClientShared &state);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  void onChangeSettings(const SettingsDelta &) override;
  void onBlur() override;
};

class ControlsTab: public SettingsTab {
 private:
 public:
  ControlsTab(ui::Button *const selectorButton,
              Settings &newSettings, ClientShared &state);

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
  void signalRead() override;
  void signalClear() override;

  void onChangeSettings(const SettingsDelta &) override;
  void onBlur() override;
};

class SettingsPage: public Page {
 private:
  Settings newSettings;

  ui::Button generalButton, playerButton, controlsButton;

  GeneralTab generalTab;
  PlayerTab playerTab;
  ControlsTab controlsTab;
  SettingsTab *currentTab;

  void switchToTab(SettingsTab *const newTab);

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
