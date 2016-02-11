#include "server.h"
#include "util/methods.h"

/**
 * Server.
 */

Server::Server() : running(true) { }

void Server::tick(float delta) {
  if (sky) sky->tick(delta);

  uptime += delta;
}

int main() {
  Server server;
  sf::Clock clock;

  while (server.running) {
    server.tick(clock.restart().asSeconds());
    sf::sleep(sf::milliseconds(16));
  }
}
