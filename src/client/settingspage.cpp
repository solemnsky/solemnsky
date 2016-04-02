#include "settingspage.h"
#include "elements/style.h"
#include "util/methods.h"

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
    debugOption(style.settings.checkbox, style.settings.debugOptionPos) {
  debugOption.description = "debug mode";
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
                  "GENERAL"),
    playerButton(style.base.normalButton,
                 {style.settings.playerButtonOffset,
                  style.settings.pageButtonHeight},
                 "PLAYER"),
    controlsButton(style.base.normalButton,
                   {style.settings.controlsButtonOffset,
                    style.settings.pageButtonHeight},
                   "CONTROLS"),

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

