/**
 * Page where the user modifies settings.
 */
#ifndef SOLEMNSKY_SETTINGSPAGE_H
#define SOLEMNSKY_SETTINGSPAGE_H

#include "elements/elements.h"

enum class SettingsPageTab {
  General, Player, Controls
};

/**
 * Widget that manages the description / tooltip / entry device for an option
 * in the settings. Can be any of a number of option types.
 */
class OptionWidget: public ui::Control {
 public:
  const struct Style {
    int fontSize = 50;
    ui::TextEntry::Style textEntryStyle;
    ui::Checkbox::Style checkboxStyle;

    sf::Color descriptionColor = sf::Color(255, 255, 255);
    sf::Vector2f entryOffset{200, 0};

    Style();
  } style;

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

class SettingsPage: public Page {
 private:
  const struct Style {

    sf::Vector2f debugChooserPos{100, 100}; // general tab
    sf::Vector2f nicknameChooserPos{100, 100}; // player tab
    /* stub */ // controls tab

    float pageButtonHeight = 800;
    float
        generalButtonOffset = 200,
        playerButtonOffset = 500,
        controlsButtonOffset = 800;

    sf::Color selectedTabButtonColor{100, 100, 100},
        unselectedTabButtonColor{132, 173, 181};
    // color of the selected tab button

    Style() { }
  } style;

  Settings newSettings;
  // we write the user-edited settings to this cache and then construct a
  // delta when we want to distribute the new settings

  SettingsPageTab currentTab;

  ui::Button generalButton, playerButton, controlsButton;
  // buttons we use to change tab

  // options on general tab
  OptionWidget debugOption;
  // options on player tab
  OptionWidget nicknameOption;
  // options on controls tab
  // stub

  /**
   * Helpers
   */
  void doForWidgets(
      const optional<SettingsPageTab> tab, // if null, apply to all widgets
      std::function<void(OptionWidget &)> f);
  void doForButtons(
      std::function<void(ui::Control &)> f);
  ui::Button &referenceButton(const SettingsPageTab tab);
  void switchToTab(const SettingsPageTab newTab);

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
