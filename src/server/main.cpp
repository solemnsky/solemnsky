/**
 * Server top-level.
 */
#include "server.h"
#include "servers/vanilla.h"

int main() {
  // TODO: commandline arguments

  // and He said,
  const sky::ArenaInitializer init("my special server");
  ServerExec(4242, init,
      [](ServerTelegraphy &telegraphy, sky::Arena &arena, sky::Sky &sky) {
        return std::make_unique<VanillaServer>(telegraphy, arena, sky);
      }).run();
  // and lo, there appeared a server
}
