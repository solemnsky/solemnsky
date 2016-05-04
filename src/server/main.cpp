/**
 * Server top-level.
 */
#include "server.h"
#include "servers/vanilla.h"

int main() {
  // TODO: commandline arguments

  // and He said,
  const sky::ArenaInit arenaInit{"my special server", "test_map"};
  const sky::SkyInitializer skyInit{};
  ServerExec(4242, arenaInit, skyInit,
             [](ServerShared &telegraphy, sky::Arena &arena, sky::Sky &sky) {
        return std::make_unique<VanillaServer>(telegraphy, arena, sky);
      }).run();
  // and lo, there appeared a server
}
