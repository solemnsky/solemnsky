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
 * Client top-level.
 */
#include "ui/control.hpp"
#include "client.hpp"

int main() {
  // TODO: commandline arguments?
  // and He said,
  
  // ui::runSFML([](const ui::AppRefs &appState) {
    // return std::make_unique<Client>(appState); 
  // });
 
  ui::ResourceLoader loader({}, {});
  loader.loadAllThreaded();
  while (!loader.getHolder()) {
    sf::sleep(sf::seconds(0.1));
  }
  appLog("done loading!");
  
  // and lo, there appeared a client
}
