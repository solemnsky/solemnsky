#include "listingpage.h"

ListingPage::ListingPage(ClientShared &state) :
    Page(state) {}

void ListingPage::onChangeSettings(const SettingsDelta &settings) { }

void ListingPage::onBlur() {

}

void ListingPage::tick(float) {

}

void ListingPage::render(ui::Frame &f) {
  drawBackground(f);
}

bool ListingPage::handle(const sf::Event &event) {
  return false;
}

void ListingPage::reset() {

}

