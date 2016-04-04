#include "settingspage.h"
#include "elements/style.h"
#include "util/methods.h"

/**
 * GeneralTab.
 */

GeneralTab::GeneralTab(const Settings &settings) :
    SettingsTab(settings),
    debugOption(style.settings.checkbox, style.settings.debugOptionPos) {
  debugOption.description = "debug mode";
  areChildren({&debugOption});
  readSettings(settings);
}

void GeneralTab::readSettings(const Settings &buffer) {
  debugOption.setValue(buffer.enableDebug);
}

void GeneralTab::writeSettings(Settings &buffer) {
  buffer.enableDebug = debugOption.getValue();
}

/**
 * PlayerTab.
 */

PlayerTab::PlayerTab(const Settings &settings) :
    SettingsTab(settings),
    nicknameOption(style.base.normalTextEntry,
                   style.settings.nicknameChooserPos) {
  nicknameOption.description = "nickname";
  areChildren({&nicknameOption});
  readSettings(settings);
}

void PlayerTab::readSettings(const Settings &settings) {
  nicknameOption.contents = settings.nickname;
}

void PlayerTab::writeSettings(Settings &settings) {
  settings.nickname = nicknameOption.contents;
}

/**
 * ControlsTab.
 */

ControlsTab::ControlsTab(const Settings &settings) :
    SettingsTab(settings) { }

void ControlsTab::readSettings(const Settings &settings) { }

void ControlsTab::writeSettings(Settings &buffer) { }

/**
 * SettingsPage.
 */

void SettingsPage::switchToTab(ui::Button &button, SettingsTab &tab) {
  currentTab.first->setActive(true);
  currentTab.second->writeSettings(newSettings);

  button.setActive(false);
  tab.readSettings(newSettings);
  currentTab = std::make_pair(&button, &tab);
}

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

    generalTab(shared.settings),
    playerTab(shared.settings),
    controlsTab(shared.settings),
    currentTab(&generalButton, &generalTab) {
  areChildren(
      {&generalButton, &playerButton, &controlsButton});
  currentTab.first->setActive(false);
}

void SettingsPage::tick(float delta) {
  currentTab.second->tick(delta);
  ui::Control::tick(delta);
}

void SettingsPage::render(ui::Frame &f) {
  drawBackground(f);
  currentTab.second->render(f);
  ui::Control::render(f);
}

bool SettingsPage::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;
  return currentTab.second->handle(event);
}

void SettingsPage::reset() {
  ui::Control::reset();
}

void SettingsPage::signalRead() {
  ui::Control::signalRead();
  currentTab.first->signalRead();
  if (generalButton.clickSignal) switchToTab(generalButton, generalTab);
  if (playerButton.clickSignal) switchToTab(playerButton, playerTab);
  if (controlsButton.clickSignal) switchToTab(controlsButton, controlsTab);
}

void SettingsPage::signalClear() {
  ui::Control::signalClear();
  currentTab.first->signalClear();
}

void SettingsPage::onBlur() {
  currentTab.second->writeSettings(newSettings);
  shared.changeSettings(SettingsDelta(shared.settings, newSettings));
}

void SettingsPage::onChangeSettings(const SettingsDelta &delta) {
  newSettings = shared.settings;
  currentTab.second->readSettings(shared.settings);
}

