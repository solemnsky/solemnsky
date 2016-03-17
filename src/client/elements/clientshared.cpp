#include "client/client.h"

/**
 * ClientUiState.
 */

ClientUiState::ClientUiState() :
    focusedPage(PageType::Home),
    gameFocusing(false),
    pageFocusing(false),
    pageFocusFactor(0),
    gameFocusFactor(0) { }

void ClientUiState::focusGame() {
  gameFocusing = true;
}

void ClientUiState::blurGame() {
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

void ClientUiState::blurPage() {
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
    client(client),
    uptime(0) { }

void ClientShared::beginGame(std::unique_ptr<Game> &&game) {
  client->beginGame(std::move(game));
}

void ClientShared::blurGame() {
  client->blurGame();
}

void ClientShared::focusGame() {
  client->focusGame();
}

void ClientShared::exitGame() {
  client->exitGame();
}

void ClientShared::focusPage(const PageType type) {
  client->focusPage(type);
}

void ClientShared::blurPage() {
  client->blurPage();
}

void ClientShared::changeSettings(const SettingsDelta &settings) {
  client->changeSettings(settings);
}
