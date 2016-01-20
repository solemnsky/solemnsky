#include "listingpage.h"

ListingPage::ListingPage(ClientState *state) : state(state) { }

void ListingPage::tick(float delta) {

}

void ListingPage::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Title), {0, 0}, {0, 0, 1600, 900});
  f.drawText({800, 600},
             {"listing page",
              "page where the user checks server listings / views stats."}, 40);
}

void ListingPage::handle(const sf::Event &event) {

}
