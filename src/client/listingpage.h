/**
 * Page where the user checks server listings / views stats.
 */
#ifndef SOLEMNSKY_LISTINGPAGE_H
#define SOLEMNSKY_LISTINGPAGE_H

#include "ui/ui.h"
#include "clientstate.h"

class ListingPage : public ui::Control {
private:
  ClientState *state;
public:
  ListingPage(ClientState *state);

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_LISTINGPAGE_H
