#include "settingspage.h"

/****
 * SettingsPage.
 */

void SettingsPage::tick(float delta) {

}

void SettingsPage::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"settings page", "where the user can modify settings"}, 40);
}

void SettingsPage::handle(const sf::Event &event) {

}

void SettingsPage::signalRead() {
}

void SettingsPage::signalClear() {
  completeSignal.clear();
}

