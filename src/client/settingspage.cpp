#include "settingspage.h"

/**
 * OptionWidget.
 */

OptionWidget::Style::Style() :
    fontSize(50),
    descriptionColor(255, 255, 255),
    entryOffset(200, -20) {
  checkboxStyle.dimensions = 40;
}

OptionWidget::OptionWidget(
    std::string *option, const sf::Vector2f &pos,
    const std::string &name, const std::string &tooltip) :
    pos(pos),
    strOption(option),
    textEntry(std::make_shared<ui::TextEntry>(
        pos + style.entryOffset, "", true, style.textEntryStyle)),
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
        pos + style.entryOffset, style.checkboxStyle)),
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
  f.drawText(pos, {name}, style.fontSize);
  if (textEntry) return textEntry->render(f);
  else return checkbox->render(f);
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
 * SettingsPage.
 */

SettingsPage::Style::Style() :
    debugChooserPos(100, 100),
    nicknameChooserPos(100, 100),
    pageButtonHeight(800),
    generalButtonOffset(700 / 6),
    playerButtonOffset(generalButtonOffset + 300 + 700 / 3),
    controlsButtonOffset(playerButtonOffset + 300 + 700 / 3),
    selectedTabButtonColor(100, 100, 100),
    unselectedTabButtonColor(132, 173, 181) { }

SettingsPage::SettingsPage(ClientShared &state) :
    Page(state),

    newSettings(shared.settings),

    currentTab(SettingsPageTab::General),
    generalButton(
        {style.generalButtonOffset, style.pageButtonHeight}, "general"),
    playerButton(
        {style.playerButtonOffset, style.pageButtonHeight}, "player"),
    controlsButton(
        {style.controlsButtonOffset, style.pageButtonHeight}, "controls"),

    debugOption(&newSettings.enableDebug, style.debugChooserPos,
                "debug", "display debug information"),
    nicknameOption(&newSettings.nickname, style.nicknameChooserPos,
                   "nickname", "your nickname in-game") {
  switchToTab(currentTab); // set the button styling
}

void SettingsPage::doForWidgets(
    const optional<SettingsPageTab> tab,
    std::function<void(OptionWidget & )> f) {
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

void SettingsPage::doForButtons(std::function<void(ui::Control &)> f) {
  f(generalButton);
  f(playerButton);
  f(controlsButton);
}

ui::Button &SettingsPage::referenceButton(const SettingsPageTab tab) {
  switch (tab) {
    case SettingsPageTab::General:
      return generalButton;
    case SettingsPageTab::Player:
      return playerButton;
    case SettingsPageTab::Controls:
      return controlsButton;
  }
}

/**
 * Page interface.
 */

void SettingsPage::onBlur() {
  doForWidgets({}, [](OptionWidget &widget) {
    widget.onBlur();
  });
  shared.changeSettings(SettingsDelta(shared.settings, newSettings));
}

void SettingsPage::onFocus() {

}

void SettingsPage::onChangeSettings(const SettingsDelta &delta) {
  newSettings = shared.settings;
  doForWidgets({}, [](OptionWidget &widget) {
    widget.onChangeSettings();
  });
}

/**
 * Game interface.
 */

void SettingsPage::tick(float delta) {
  doForWidgets(currentTab, [delta](OptionWidget &widget) {
    widget.tick(delta);
  });

  generalButton.tick(delta);
  playerButton.tick(delta);
  controlsButton.tick(delta);
}

void SettingsPage::render(ui::Frame &f) {
  drawBackground(f);

  doForWidgets(currentTab, [&f](auto &widget) { widget.render(f); });
  doForButtons([&f](auto &button) { button.render(f); });
}

bool SettingsPage::handle(const sf::Event &event) {
  bool eventHandled = false;
  doForWidgets(currentTab, [&](auto &widget) {
    if (!eventHandled) eventHandled = widget.handle(event);
  });
  if (eventHandled) return true;
  doForButtons([&](auto &button) {
    if (!eventHandled) eventHandled = button.handle(event);
  });
  return eventHandled;
}

void SettingsPage::switchToTab(const SettingsPageTab newTab) {
  auto &oldButton = referenceButton(currentTab),
      &newButton = referenceButton(newTab);

  oldButton.setActive(true);
  oldButton.style.baseColor = style.unselectedTabButtonColor;
  newButton.setActive(false);
  newButton.style.baseColor = style.selectedTabButtonColor;

  doForWidgets(currentTab, [](OptionWidget &widget) {
    widget.onBlur();
  });
  currentTab = newTab;
}

void SettingsPage::signalRead() {
  doForWidgets({}, [](OptionWidget &widget) {
    widget.signalRead();
  });

  if (generalButton.clickSignal) {
    switchToTab(SettingsPageTab::General);
    return;
  }
  if (playerButton.clickSignal) {
    switchToTab(SettingsPageTab::Player);
    return;
  }
  if (controlsButton.clickSignal) {
    switchToTab(SettingsPageTab::Controls);
    return;
  }
}

void SettingsPage::signalClear() {
  doForWidgets({}, [](OptionWidget &widget) {
    widget.signalClear();
  });

  doForButtons([](auto &button) { button.signalClear(); });
}
