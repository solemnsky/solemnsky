#include "settingspage.h"

/****
 * SettingsPage.
 */

SettingsPage::SettingsPage(ClientState *state) : state(state) { }

void SettingsPage::tick(float delta) {

}

void SettingsPage::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"settings page", "page where the user modifies settings"}, 40);
}

void SettingsPage::handle(const sf::Event &event) {

}

void SettingsPage::signalRead() {
}

void SettingsPage::signalClear() {
  completeSignal.clear();
}

