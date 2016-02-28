#include "settingspage.h"

ui::TextEntry::Style SettingsPage::Style::textEntryStyle() const {
  ui::TextEntry::Style textStyle;
  textStyle.fontSize = fontSize;
  textStyle.dimensions = textEntryDimensions;
  return textStyle;
}

SettingsPage::SettingsPage(ClientShared &state) :
    Page(state),

    currentTab(SettingsPageTab::General),
    generalButton(
        {style.pageButtonHeight, style.generalButtonOffset}, "general"),
    playerButton(
        {style.pageButtonHeight, style.playerButtonOffset}, "player"),
    controlsButton(
        {style.pageButtonHeight, style.controlsButtonOffset}, "controls"),

    debugChooser(),
    nicknameChooser(),

    newSettings(shared.settings),
    nicknameChooser(style.nicknameEntryPos, "", true,
                    style.textEntryStyle()) {
  nicknameChooser.contents = shared.settings.nickname;
}

void SettingsPage::doForTabWidgets(
    const SettingsPageTab tab,
    std::function<void(ui::Control &)> f) {
  switch (tab) {
    case SettingsPageTab::General: {
      f(debugChooser);
      break;
    }
    case SettingsPageTab::Player: {
      f(nicknameChooser);
      break;
    }
    case SettingsPageTab::Controls: {
      break;
    }
  }
}


void SettingsPage::writeToSettings() {
  shared.changeSettings(SettingsDelta(shared.settings, newSettings));
}

/**
 * Page interface.
 */

void SettingsPage::onBlur() {
  newSettings.nickname = nicknameChooser.contents;

  writeToSettings();
  nicknameChooser.reset();
}

void SettingsPage::onFocus() {

}

void SettingsPage::onChangeSettings(const SettingsDelta &delta) {
  if (delta.nickname) nicknameChooser.contents = *delta.nickname;
  newSettings = shared.settings;
}

/**
 * Game interface.
 */

void SettingsPage::tick(float delta) {
  nicknameChooser.tick(delta);
}

void SettingsPage::render(ui::Frame &f) {
  drawBackground(f);

  f.drawText(style.nicknameDescPos, {style.nicknameDesc}, style.fontSize,
             style.descColor);
  nicknameChooser.render(f);
}

bool SettingsPage::handle(const sf::Event &event) {
  return nicknameChooser.handle(event);
}

void SettingsPage::signalRead() {
  if (nicknameChooser.inputSignal) {
    newSettings.nickname = nicknameChooser.contents;
    writeToSettings();
  }
}

void SettingsPage::signalClear() {
  nicknameChooser.signalClear();
}
