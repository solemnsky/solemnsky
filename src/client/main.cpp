/**
 * Client top-level.
 */
#include "ui/control.h"
#include "client.h"

int main() {
  // TODO: commandline arguments?

  // and He said,
  ui::runSFML([]() { return std::make_unique<Client>(); });
  // and lo, there appeared a client
}
