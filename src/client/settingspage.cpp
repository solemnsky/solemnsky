#include "settingspage.h"

ui::TextEntry::Style textEntryStyle(const SettingsPage::Style &style) {
  ui::TextEntry::Style textStyle;
  textStyle.fontSize = style.fontSize;
  textStyle.dimensions = style.textEntryDimensions;
  return textStyle;
}

SettingsPage::SettingsPage(ClientShared &state) :
    Page(state),
    style(),
    newSettings(shared.settings),
    nicknameChooser(style.nicknameEntryPos, "", true, textEntryStyle(style)) {
  nicknameChooser.contents = shared.settings.nickname;
}

void SettingsPage::writeToSettings() {
  shared.onChangeSettings(SettingsDelta(shared.settings, newSettings));
}

/**
 * Page interface.
 */

void SettingsPage::onLooseFocus() {
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
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
  f.drawText({800, 600},
             {"settings page", "page where the user modifies settings"}, 40);

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
  }
}

void SettingsPage::signalClear() {

}
