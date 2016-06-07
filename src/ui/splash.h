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
 * Splash screen; loads resources and enters the main menu on user input.
 */
#pragma once
#include <memory>
#include "widgets/textlog.h"
#include "control.h"

namespace ui {

namespace detail {
/**
 * Supplied with a constructor for a main Control, this loads resources while
 * displaying a splash screen and fades into the main Control when resources
 * are ready. It also relays the wrapped control's `quitting` flag.
 */
class SplashScreen: public Control {
 private:
  ResourceLoader loader;
  optional<AppResources> loadedResources;
  const sf::Font &defaultFont;

  double animBegin;
  TextFormat readyText;

  std::function<Control(const AppState &)> mkApp;
  optional<Control> control;

 public:
  SplashScreen(AppState &appState,
               std::function<Control(const AppState &)> mkApp);

  bool poll() override final;
  void tick(const TimeDiff delta) override final;
  void render(Frame &f) override final;
  bool handle(const sf::Event &event) override final;
};
}

}
