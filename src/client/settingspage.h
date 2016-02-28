/**
 * Page where the user modifies settings.
 */
#ifndef SOLEMNSKY_SETTINGSPAGE_H
#define SOLEMNSKY_SETTINGSPAGE_H

#include "elements/elements.h"

enum class SettingsPageTab {
  General, Player, Controls
};


class SettingsPage: public Page {
 public:
  struct Style {
    int fontSize = 50;
    sf::Vector2f textEntryDimensions{400, 60};

    sf::Color descColor = sf::Color(255, 255, 255);
    sf::Vector2f nicknameDescPos{100, 100};
    std::string nicknameDesc = "nickname:";
    sf::Vector2f nicknameEntryPos{300, 100};

    float pageButtonHeight = 800;
    float
        generalButtonOffset = 1,
        playerButtonOffset = 1,
        controlsButtonOffset = 1;

    Style() { }
  } style;

 private:
  Settings newSettings;
  // we write the user-edited settings to this cache and then construct a
  // delta when we want to distribute the new settings

  SettingsPageTab currentTab;
  ui::Button generalButton, playerButton, controlsButton;

 public:
  SettingsPage(ClientShared &state);

  ui::TextEntry nicknameChooser;

  /**
   * Writing to settings.
   */
  void writeToSettings();

  /**
   * Page interface.
   */
  void onBlur() override;
  void onFocus() override;
  void onChangeSettings(const SettingsDelta &) override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_SETTINGSPAGE_H
