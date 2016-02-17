#include "listingpage.h"

ListingPage::ListingPage(ClientShared &state) :
    Page(state) {}

void ListingPage::onBlur() {

}

void ListingPage::onFocus() {

}

void ListingPage::onChangeSettings(const SettingsDelta &settings) { }

void ListingPage::tick(float) {

}

void ListingPage::render(ui::Frame &f) {
  drawBackground(f);
  f.drawText({800, 600},
             {"listing page",
              "page where the user checks server listings / views stats."}, 40);
}

bool ListingPage::handle(const sf::Event &event) {
  return false;
}

