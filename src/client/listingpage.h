/**
 * Page where the user checks server listings / views stats.
 */
#ifndef SOLEMNSKY_LISTINGPAGE_H
#define SOLEMNSKY_LISTINGPAGE_H

#include "ui/control.h"
#include "clientstate.h"
#include "page.h"

class ListingPage : public Page {
private:
  ClientState *state;
public:
  ListingPage(ClientState *state);

  /**
   * Page interface.
   */
  void reset() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_LISTINGPAGE_H
