/**
 * Client top-level.
 */
#include "ui/control.h"
#include "client.h"

int main() {
  // TODO: commandline arguments?
  // and He said,
  ui::runSFML([](ui::AppState &appState) {
    return std::make_unique<Client>(appState);
  });
  // and lo, there appeared a client
}
