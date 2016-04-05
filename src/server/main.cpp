/**
 * Server top-level.
 */
#include "server.h"
#include "servers/vanilla.h"

int main() {
  // TODO: commandline arguments

  // and He said,
  runServer(
      [](ServerTelegraphy &telegraphy, sky::Arena &arena, sky::Sky &sky) {
        return std::make_unique<VanillaServer>(telegraphy, arena, sky);
      });
  // and lo, there appeared a server
}
