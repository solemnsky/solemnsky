#include "client/client.h"

/**
 * ClientUiState.
 */

ClientUiState::ClientUiState() :
    focusedPage(PageType::Home),
    gameFocusing(false),
    pageFocusing(false),
    pageFocusFactor(0, 1, 0),
    gameFocusFactor(0, 1, 0) { }

void ClientUiState::focusGame() {
  unfocusPage();
  gameFocusing = true;
}

void ClientUiState::unfocusGame() {
  gameFocusing = false;
}

void ClientUiState::focusPage(PageType page) {
  if (pageFocusFactor != 0) {
    pageFocusing = false;
  } else {
    pageFocusing = true;
    focusedPage = page;
  }
}

void ClientUiState::unfocusPage() {
  pageFocusing = false;
}

void ClientUiState::tick(float delta) {
  pageFocusFactor += pageFocusAnimSpeed * delta * (pageFocusing ? 1 : -1);
  gameFocusFactor += gameFocusAnimSpeed * delta * (gameFocusing ? 1 : -1);
}

bool ClientUiState::pageFocused() const { return pageFocusFactor == 1; }

bool ClientUiState::gameFocused() const { return gameFocusFactor == 1; }

bool ClientUiState::menuFocused() const {
  return pageFocusFactor == 0 and gameFocusFactor == 0;
}

/**
 * ClientShared.
 */

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

