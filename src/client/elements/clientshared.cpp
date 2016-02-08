#include "client/client.h"

ClientShared::ClientShared(Client *client) :
    client(client) { }

bool ClientShared::gameInFocus() {
  if (game) return game->inFocus;
  return false;
}

void ClientShared::beginGame(std::unique_ptr<Game> &&game) {
  unfocusPage();
  client->beginGame(game);
}

void ClientShared::focusPage(const PageType type) {
  client->focusPage(type);
}

void ClientShared::unfocusPage() {
  client->unfocusPage();
}
