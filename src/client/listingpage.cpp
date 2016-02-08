#include "listingpage.h"

ListingPage::ListingPage(ClientShared &state) :
    Page(state) {}

void ListingPage::reset() {

}

void ListingPage::tick(float) {

}

void ListingPage::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
  f.drawText({800, 600},
             {"listing page",
              "page where the user checks server listings / views stats."}, 40);
}

bool ListingPage::handle(const sf::Event &event) {
  return false;
}

