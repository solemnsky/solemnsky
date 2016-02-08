#include "client/client.h"

ClientShared::ClientShared(Client *client) :
    client(client) { }

void ClientShared::beginGame(std::unique_ptr<Game> &&game) {
  unfocusPage();
  client->beginGame(std::move(game));
}

void ClientShared::unfocusGame() {
  client->unfocusGame();
}

void ClientShared::focusPage(const PageType type) {
  client->focusPage(type);
}

void ClientShared::unfocusPage() {
  client->unfocusPage();
}

