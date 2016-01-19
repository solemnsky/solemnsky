#include "listingpage.h"

ListingPage::ListingPage(ClientState *state) : state(state) { }

void ListingPage::tick(float delta) {

}

void ListingPage::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"listing page",
              "page where the user checks server listings / views stats."}, 40);
}

void ListingPage::handle(const sf::Event &event) {

}
