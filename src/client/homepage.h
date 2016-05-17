/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * Home page.
 */
#ifndef SOLEMNSKY_HOMEPAGE_H
#define SOLEMNSKY_HOMEPAGE_H

#include "elements/elements.h"
#include "ui/widgets/widgets.h"

class HomePage: public Page {
 private:
  ui::Button tutorialButton;
  ui::Button localhostButton;
  ui::Button remoteButton;

 public:
  HomePage(ClientShared &state);

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
  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_HOMEPAGE_H
