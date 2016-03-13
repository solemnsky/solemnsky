/**
 * Page where the user checks server listings / views stats.
 */
#ifndef SOLEMNSKY_LISTINGPAGE_H
#define SOLEMNSKY_LISTINGPAGE_H

#include "elements/elements.h"

class ListingPage : public Page {
public:
  ListingPage(ClientShared &state);

  /**
   * Page interface.
   */
  void onBlur() override;
  void onFocus() override;
  virtual void onChangeSettings(const SettingsDelta &settings) override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_LISTINGPAGE_H
