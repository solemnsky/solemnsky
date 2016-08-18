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
#include "widgets/textlog.hpp"
#include "control.hpp"

namespace ui {

namespace detail {
/**
 * Supplied with a constructor for a main Control, this loads resources while
 * displaying a splash screen and fades into the main Control when resources
 * are ready. It also relays the wrapped control's `quitting` flag.
 *
 * We can't use `resources` here! Our job is to create them for the child app.
 */
class SplashScreen : public Control {
 private:
  // App constructor.
  std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp;

  // Loader and splash screen.
  ResourceLoader loader;
  sf::Font const *defaultFont;
  sf::Texture const *background;
  double animBegin;

  // App construction.
  void constructApp(const AppResources &resources);
  optional<AppResources> loadedResources;
  optional<AppRefs> initializedReferences;
  std::unique_ptr<Control> control;

 public:
  SplashScreen(const AppRefs &appState,
               std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp);

  // Control impl.
  void poll() override final;
  void tick(const TimeDiff delta) override final;
  void render(Frame &f) override final;
  bool handle(const sf::Event &event) override final;

};

}

}

