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
    textEntry.reset();
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
}

void SettingsPage::render(ui::Frame &f) {
  drawBackground(f);

  doForWidgets(currentTab, [&f](OptionWidget &widget) {
    widget.render(f);
  });

  generalButton.render(f);
  playerButton.render(f);
  controlsButton.render(f);
}

bool SettingsPage::handle(const sf::Event &event) {
  bool widgetHandle = false;
  doForWidgets(currentTab, [&](OptionWidget &widget) {
    if (!widgetHandle) widgetHandle = widget.handle(event);
  });
  if (widgetHandle) return true;
  if (generalButton.handle(event)) return true;
  if (playerButton.handle(event)) return true;
  return controlsButton.handle(event);
}

void SettingsPage::switchToTab(const SettingsPageTab newTab) {
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

  generalButton.signalClear();
  generalButton.playerButton();
  generalButton.controlsButton();
}
