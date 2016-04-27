/**
 * Client top-level.
 */
#include "ui/control.h"
#include "client.h"

int main() {
  // TODO: commandline arguments?
  sf::Window window;
  window.create(sf::VideoMode(800, 600), "My window");

  // and He said,
  // ui::runSFML([](ui::AppState &appState) {
    // return std::make_unique<Client>(appState);
  // });
  // and lo, there appeared a client
}
