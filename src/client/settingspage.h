/**
 * Page where the user modifies settings.
 */
#ifndef SOLEMNSKY_SETTINGSPAGE_H
#define SOLEMNSKY_SETTINGSPAGE_H

#include <client/ui/widgets/radiobutton.h>
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

    ui::TextEntry::Style textEntryStyle() const;

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

  ui::RadioButton debugChooser; // general tab
  ui::TextEntry nicknameChooser; // player tab
  /* stub */ // controls tab

  /**
   * Helpers
   */
  // invoke a function on every widget in a certain tab
  void doForTabWidgets(
      const SettingsPageTab tab, std::function<void(ui::Control &)> f);

  void writeToSettings();

 public:
  SettingsPage(ClientShared &state);

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
