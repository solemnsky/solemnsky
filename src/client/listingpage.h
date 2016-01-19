/**
 * Listing page, where the user can see a server listing and choose the
 * server he wants to join.
 */
#ifndef SOLEMNSKY_LISTINGPAGE_H
#define SOLEMNSKY_LISTINGPAGE_H

#include "ui/ui.h"

class ListingPage : ui::Control {
private:

public:
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_LISTINGPAGE_H
