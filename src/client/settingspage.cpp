#include "settingspage.h"

/**
 * OptionWidget.
 */

OptionWidget::Style::Style() {
  checkboxStyle.dimensions = 40;
}

OptionWidget::OptionWidget(
    std::string *option, const sf::Vector2f &pos,
    const std::string &name, const std::string &tooltip) :
    pos(pos),
    strOption(option),
    textEntry(std::make_shared<ui::TextEntry>(
        pos + style.entryOffset, name, true, style.textEntryStyle) { }

OptionWidget::OptionWidget(
    bool *option, const sf::Vector2f &pos,
    const std::string &name, const std::string &tooltip) :
    pos(pos),
    boolOption(option),
    checkbox(std::make_shared<ui::Checkbox>(
        pos + style.entryOffset, name, true, style.checkboxStyle)) { }

void OptionWidget::onChangeSettings() {
  if (textEntry) {
    textEntry->contents = *strOption;
  }
  if (checkbox) {
    checkbox->setValue(*boolOption);
  }
}

void OptionWidget::onBlur() {
  if (textEntry) {
    *strOption = textEntry->contents;
  }
}

void OptionWidget::tick(float delta) {
  if (textEntry) return textEntry->tick(delta);
  if (checkbox) return checkbox->tick(delta);
}

void OptionWidget::render(ui::Frame &f) {
  if (textEntry) return textEntry->render(f);
  if (checkbox) return checkbox->render(f);
}

bool OptionWidget::handle(const sf::Event &event) {
  if (textEntry) return textEntry->handle(event);
  if (checkbox) return checkbox->handle(event);
}

void OptionWidget::signalRead() {
  if (textEntry) {
    textEntry->signalRead();
    if (textEntry->inputSignal) *strOption = *textEntry->inputSignal;
    return;
  }

  if (checkbox) {
    checkbox->signalRead();
    if (checkbox->clickSignal) *boolOption = checkbox->getValue();
  }
}

void OptionWidget::signalClear() {
  if (textEntry) return textEntry->signalClear();
  if (checkbox) return checkbox->signalClear();
}

/**
 * SettingsPage.
 */

SettingsPage::SettingsPage(ClientShared &state) :
    Page(state),

    newSettings(shared.settings),

    currentTab(SettingsPageTab::General),
    generalButton(
        {style.pageButtonHeight, style.generalButtonOffset}, "general"),
    playerButton(
        {style.pageButtonHeight, style.playerButtonOffset}, "player"),
    controlsButton(
        {style.pageButtonHeight, style.controlsButtonOffset}, "controls"),

    debugOption(&newSettings.enableDebug, style.debugChooserPos,
                "debug", "display debug information"),
    nicknameOption(&newSettings.nickname, style.nicknameChooserPos,
                   "nickname", "your nickname in-game") { }

void SettingsPage::doForWidgets(
    const optional<SettingsPageTab> tab,
    std::function<void(ui::Control &)> f) {
  if (!tab) {
    f(debugOption);
    f(nicknameOption);
    return;
  }

  switch (*tab) {
    case SettingsPageTab::General: {
      f(debugOption);
      break;
    }
    case SettingsPageTab::Player: {
      f(nicknameOption);
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
  doForWidgets({}, ())

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
