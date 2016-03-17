#include "settingspage.h"
#include "elements/style.h"
#include "util/methods.h"

/**
 * OptionWidget.
 */

OptionWidget::OptionWidget(
    std::string *option, const sf::Vector2f &pos,
    const std::string &name, const std::string &tooltip) :
    pos(pos),
    strOption(option),
    textEntry(std::make_shared<ui::TextEntry>(
        style.settings.textEntry,
        pos + style.settings.entryOffset, "", true)),
    name(name),
    tooltip(tooltip) {
  onChangeSettings();
}

OptionWidget::OptionWidget(
    bool *option, const sf::Vector2f &pos,
    const std::string &name, const std::string &tooltip) :
    pos(pos),
    boolOption(option),
    checkbox(std::make_shared<ui::Checkbox>(
        style.settings.checkbox,
        pos + style.settings.entryOffset)),
    name(name),
    tooltip(tooltip) {
  onChangeSettings();
}

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
    textEntry->reset();
    return;
  }
  if (checkbox) {
    *boolOption = checkbox->getValue();
    checkbox->reset();
    return;
  }
}

void OptionWidget::tick(float delta) {
  if (textEntry) return textEntry->tick(delta);
  if (checkbox) return checkbox->tick(delta);
}

void OptionWidget::render(ui::Frame &f) {
  f.drawText(pos, {name}, style.base.normalFontSize, style.base.textColor);
  if (textEntry) textEntry->render(f);
  else checkbox->render(f);
}

bool OptionWidget::handle(const sf::Event &event) {
  if (textEntry) return textEntry->handle(event);
  else return checkbox->handle(event);
}

void OptionWidget::signalRead() {
  if (textEntry) {
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
 * SettingsTab.
 */

SettingsTab::SettingsTab(ui::Button *const selectorButton,
                         Settings &newSettings,
                         ClientShared &state) :
    Page(state),
    selectorButton(selectorButton),
    newSettings(newSettings) { }

/**
 * GeneralTab.
 */

GeneralTab::GeneralTab(ui::Button *const selectorButton,
                       Settings &newSettings, ClientShared &state) :
    SettingsTab(selectorButton, newSettings, state),
    debugOption(&newSettings.enableDebug, style.settings.debugChooserPos,
                "debug", "display debug information") {
  areChildren({&debugOption});
}

void GeneralTab::tick(float delta) {
  ui::Control::tick(delta);
}

void GeneralTab::render(ui::Frame &f) {
  ui::Control::render(f);
}

bool GeneralTab::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void GeneralTab::reset() {
  ui::Control::reset();
}

void GeneralTab::signalRead() {
  ui::Control::signalRead();
}

void GeneralTab::signalClear() {
  ui::Control::signalClear();
}

void GeneralTab::onChangeSettings(const SettingsDelta &) {

}

void GeneralTab::onBlur() {

}

/**
 * PlayerTab.
 */

PlayerTab::PlayerTab(ui::Button *const selectorButton,
                     Settings &newSettings, ClientShared &state) :
    SettingsTab(selectorButton, newSettings, state),
    nicknameOption(&newSettings.nickname, style.settings.nicknameChooserPos,
                   "nickname", "your nickname in-tutorial") {
  areChildren({&nicknameOption});
}

void PlayerTab::tick(float delta) {
  ui::Control::tick(delta);
}

void PlayerTab::render(ui::Frame &f) {
  ui::Control::render(f);
}

bool PlayerTab::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void PlayerTab::reset() {
  ui::Control::reset();
}

void PlayerTab::signalRead() {
  ui::Control::signalRead();
}

void PlayerTab::signalClear() {
  ui::Control::signalClear();
}

void PlayerTab::onChangeSettings(const SettingsDelta &) {

}

void PlayerTab::onBlur() {

}

/**
 * ControlsTab.
 */

ControlsTab::ControlsTab(ui::Button *const selectorButton,
                         Settings &newSettings, ClientShared &state) :
    SettingsTab(selectorButton, newSettings, state) { }

void ControlsTab::tick(float delta) {
  ui::Control::tick(delta);
}

void ControlsTab::render(ui::Frame &f) {
  ui::Control::render(f);
}

bool ControlsTab::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void ControlsTab::reset() {
  ui::Control::reset();
}

void ControlsTab::signalRead() {
  ui::Control::signalRead();
}

void ControlsTab::signalClear() {
  ui::Control::signalClear();
}

void ControlsTab::onChangeSettings(const SettingsDelta &) {

}

void ControlsTab::onBlur() {

}

/**
 * SettingsPage.
 */

SettingsPage::SettingsPage(ClientShared &shared) :
    Page(shared),

    newSettings(shared.settings),

    generalButton(style.base.normalButton,
                  {style.settings.generalButtonOffset,
                   style.settings.pageButtonHeight},
                  "general"),
    playerButton(style.base.normalButton,
                 {style.settings.playerButtonOffset,
                  style.settings.pageButtonHeight},
                 "player"),
    controlsButton(style.base.normalButton,
                   {style.settings.controlsButtonOffset,
                    style.settings.pageButtonHeight},
                   "controls"),

    generalTab(&generalButton, newSettings, shared),
    playerTab(&playerButton, newSettings, shared),
    controlsTab(&controlsButton, newSettings, shared),
    currentTab((SettingsTab *) &generalTab) {
  areChildren(
      {&generalButton, &playerButton, &controlsButton});
  currentTab->selectorButton->setActive(false);
}

void SettingsPage::tick(float delta) {
  currentTab->tick(delta);
  ui::Control::tick(delta);
}

void SettingsPage::render(ui::Frame &f) {
  drawBackground(f);
  currentTab->render(f);
  ui::Control::render(f);
}

bool SettingsPage::handle(const sf::Event &event) {
  if (currentTab->handle(event)) return true;
  return ui::Control::handle(event);
}

void SettingsPage::switchToTab(SettingsTab *const newTab) {
  newTab->selectorButton->setActive(false);
  currentTab->selectorButton->setActive(true);
  currentTab->onBlur();
  currentTab = newTab;
}

void SettingsPage::reset() {
  ui::Control::reset();
}

void SettingsPage::signalRead() {
  ui::Control::signalRead();
  currentTab->signalRead();
  if (generalButton.clickSignal) {
    switchToTab(&generalTab);
    return;
  }
  if (playerButton.clickSignal) {
    switchToTab(&playerTab);
    return;
  }
  if (controlsButton.clickSignal) {
    switchToTab(&controlsTab);
    return;
  }
}

void SettingsPage::signalClear() {
  ui::Control::signalClear();
  currentTab->signalClear();
}

void SettingsPage::onBlur() {
  shared.changeSettings(SettingsDelta(shared.settings, newSettings));
}

void SettingsPage::onChangeSettings(const SettingsDelta &delta) {
  newSettings = shared.settings;
  currentTab->onChangeSettings(delta);
}

