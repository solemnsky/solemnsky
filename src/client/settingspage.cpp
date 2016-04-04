#include "settingspage.h"
#include "elements/style.h"
#include "util/methods.h"

/**
 * GeneralTab.
 */

GeneralTab::GeneralTab(const Settings &settings) :
    SettingsTab(settings),
    debugOption(style.settings.checkbox, style.settings.debugOptionPos) {
  debugOption.setDescription(optional<std::string>("debug mode"));
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
  nicknameOption.persistent = true;
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
    SettingsTab(settings),
    boundActions{sky::Action::Left,
                 sky::Action::Right,
                 sky::Action::Thrust,
                 sky::Action::Reverse} {

  sf::Vector2f pos = style.settings.nicknameChooserPos;

  for (sky::Action action : boundActions) {
    bindingChoosers.emplace(
        std::piecewise_construct,
        std::forward_as_tuple(action),
        std::forward_as_tuple(style.base.normalButton, pos));

    auto &selector = bindingChoosers.at(action);
    selector.setDescription(sky::showAction(action));
    areChildren({&selector});

    pos += {0, 80};
  }

  readSettings(settings);
}

void ControlsTab::readSettings(const Settings &settings) {
  for (const sky::Action action : boundActions) {
    bindingChoosers.at(action).setValue(
        settings.bindings.lookupBinding(action));
  }
}

void ControlsTab::writeSettings(Settings &settings) {
  settings.bindings.skyBindings.clear();
  for (const sky::Action action : boundActions) {
    const auto &chooser = bindingChoosers.at(action);
    if (const auto binding = bindingChoosers.at(action).getValue()) {
      settings.bindings.skyBindings.emplace(binding.get(), action);
    }
  }
}

/**
 * SettingsPage.
 */

void SettingsPage::switchToTab(ui::Button &button, SettingsTab &tab) {
  currentTab.first->setActive(true);
  currentTab.second->reset();
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
  currentTab.second->signalRead();
  if (generalButton.clickSignal) switchToTab(generalButton, generalTab);
  if (playerButton.clickSignal) switchToTab(playerButton, playerTab);
  if (controlsButton.clickSignal) switchToTab(controlsButton, controlsTab);
}

void SettingsPage::signalClear() {
  ui::Control::signalClear();
  currentTab.second->signalClear();
}

void SettingsPage::onBlur() {
  currentTab.second->reset();
  currentTab.second->writeSettings(newSettings);
  shared.changeSettings(SettingsDelta(shared.settings, newSettings));
}

void SettingsPage::onChangeSettings(const SettingsDelta &delta) {
  newSettings = shared.settings;
  currentTab.second->readSettings(shared.settings);
}

