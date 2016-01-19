#include "listingpage.h"

void ListingPage::tick(float delta) {

}

void ListingPage::render(ui::Frame &f) {
  f.drawText({800, 450},
             {"listing page", "where the user can see a server listing"}, 40);
}

void ListingPage::handle(const sf::Event &event) {

}
