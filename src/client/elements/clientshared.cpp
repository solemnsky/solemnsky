#include "client/client.h"
#include "style.h"

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
  pageFocusFactor += style.menu.pageFocusAnimSpeed
      * delta * (pageFocusing ? 1 : -1);
  gameFocusFactor += style.menu.gameFocusAnimSpeed
      * delta * (gameFocusing ? 1 : -1);
}

bool ClientUiState::pageFocused() const { return pageFocusFactor == 1; }

bool ClientUiState::gameFocused() const { return gameFocusFactor == 1; }

bool ClientUiState::menuFocused() const {
  return pageFocusFactor == 0 and gameFocusFactor == 0;
}

/**
 * ClientShared.
 */

ClientShared::ClientShared(ui::AppState &appState, Client &client) :
    client(client),
    appState(appState) { }

template<typename T>
optional<std::pair<T, bool>>
bindingFromEvent(const sf::Event &event,
                 const std::map<sf::Keyboard::Key, T> &map) {
  bool value = event.type == sf::Event::KeyPressed;
  if (value or event.type == sf::Event::KeyReleased) {
    const auto &action = map.find(event.key.code);
    if (action != map.end())
      return {std::pair<T, bool>(action->second, value)};
  }

  return {};
};

optional<std::pair<sky::Action, bool>>
ClientShared::triggerSkyAction(const sf::Event &event) const {
  return bindingFromEvent(event, settings.bindings.skyBindings);
}

optional<std::pair<ClientAction, bool>>
ClientShared::triggerClientAction(const sf::Event &event) const {
  return bindingFromEvent(event, settings.bindings.clientBindings);
}

void ClientShared::beginGame(std::unique_ptr<Game> &&game) {
  client.beginGame(std::move(game));
}

void ClientShared::blurGame() {
  client.blurGame();
}

void ClientShared::focusGame() {
  client.focusGame();
}

void ClientShared::exitGame() {
  client.exitGame();
}

void ClientShared::focusPage(const PageType type) {
  client.focusPage(type);
}

void ClientShared::blurPage() {
  client.blurPage();
}

void ClientShared::changeSettings(const SettingsDelta &settings) {
  client.changeSettings(settings);
}


