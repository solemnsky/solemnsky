/**
 * Page where the user modifies settings.
 */
#ifndef SOLEMNSKY_SETTINGSPAGE_H
#define SOLEMNSKY_SETTINGSPAGE_H

#include "elements/elements.h"

/**
 * Widget that manages the description / tooltip / entry device for an option
 * in the settings. Can be any of a number of option types.
 */
class OptionWidget: public ui::Control {
 private:
  sf::Vector2f pos;
  std::string name, tooltip;

  // invariant: exactly one of these pairs is non-null
  std::string *strOption;
  std::shared_ptr<ui::TextEntry> textEntry;

  bool *boolOption;
  std::shared_ptr<ui::Checkbox> checkbox;

 public:
  OptionWidget() = delete;
  // a widget that chooses a string, using a TextEntry
  OptionWidget(std::string *option, const sf::Vector2f &pos,
               const std::string &name, const std::string &tooltip);
  // a widget that chooses a bool, using a Checkbox
  OptionWidget(bool *option, const sf::Vector2f &pos,
               const std::string &name, const std::string &tooltip);

  void onChangeSettings();
  void onBlur(); // reset your UI state and write the option

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void signalRead() override;
  void signalClear() override;
};

class SettingsTab: public Page {
 private:
  Settings &newSettings;
 public:
  SettingsTab() = delete;
  SettingsTab(ui::Button *const selectorButton,
              Settings &newSettings,
              ClientShared &state);

  ui::Button *const selectorButton;
};

class GeneralTab: public SettingsTab {
 private:
  OptionWidget debugOption;
 public:
  GeneralTab(ui::Button *const selectorButton,
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

class PlayerTab: public SettingsTab {
 private:
  OptionWidget nicknameOption;
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

#endif //SOLEMNSKY_SETTINGSPAGE_H
