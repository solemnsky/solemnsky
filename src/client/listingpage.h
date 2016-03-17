/**
 * Server listing page.
 */
#pragma once
#include "elements/elements.h"

class ListingPage : public Page {
public:
  ListingPage(ClientShared &state);

  /**
   * Page interface.
   */
  void onChangeSettings(const SettingsDelta &settings) override;
  void onBlur() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;
  void reset() override;
};
