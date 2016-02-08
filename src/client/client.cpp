#include "client.h"
#include "util/methods.h"

/**
 * ClientUiState.
 */

ClientUiState::ClientUiState() :
    focusedPage(PageType::Home),
    gameFocusing(false),
    pageFocusing(false),
    pageFocusFactor(0, 1, 0),
    gameFocusFactor(0, 1, 0) { }

bool ClientUiState::pageFocused() { return pageFocusFactor == 1; }

bool ClientUiState::gameFocused() { return gameFocusFactor == 1; }

bool ClientUiState::menuFocused() {
  return pageFocusFactor == 0 &&
         gameFocusFactor == 0;
}

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

/**
 * Client.
 */
ui::Button::Style backButtonStyle() {
  ui::Button::Style style;
  style.fontSize = 50;
  return style;
}

Client::Client() :
    shared(this),
    homePage(shared),
    listingPage(shared),
    settingsPage(shared),
    backButton({1500, 850}, "go back", backButtonStyle()) { }

void Client::forAllPages(std::function<void(Page &)> f) {
  f(homePage);
  f(listingPage);
  f(settingsPage);
}

Page &Client::referencePage(const PageType type) {
  switch (type) {
    case PageType::Home:
      return homePage;
    case PageType::Settings:
      return settingsPage;
    case PageType::Listing:
      return listingPage;
  }
}

void Client::drawPage(ui::Frame &f, const PageType type,
                      const sf::Vector2f &offset, const std::string &name,
                      ui::Control &page) {
  const float focusFactor = uiState.pageFocusFactor;

  float alpha, scale, offsetAmnt, titleAlpha;
  if (type == uiState.focusedPage) {
    alpha = 1;
    scale = linearTween(style.unfocusedPageScale, 1,
                        uiState.pageFocusFactor);
    offsetAmnt = linearTween(1, 0, focusFactor);
    titleAlpha = linearTween(1, 0, focusFactor);
  } else {
    alpha = linearTween(1, 0, focusFactor);
    scale = style.unfocusedPageScale;
    offsetAmnt = 1;
    titleAlpha = 1;
  }

  if (alpha == 0) return;

  sf::Transform &transform =
      sf::Transform().translate(offsetAmnt * offset).scale(scale, scale);

  f.withAlpha(alpha, [&]() {
    f.withAlpha(titleAlpha, [&]() {
      f.drawText(sf::Vector2f(0, -40) + offsetAmnt * offset, {name}, 40);
    });
    f.withTransform(transform, [&]() {
      page.render(f);
    });
  });

  pageRects.push_back({transform.transformRect({0, 0, 1600, 900}), type});
}

void Client::attachState() {
  shared.appState = appState;
  forAllPages([](Page &page) { page.attachClientState(); });
}

void Client::tick(float delta) {
  shared.uptime += delta;

  if (shared.game) {
    shared.game->tick(delta);
    if (shared.game->concluded) {
      shared.game = nullptr;
      uiState.unfocusGame();
    }
  }

  uiState.tick(delta);

  forAllPages([&delta](Page &page) { page.tick(delta); });
  backButton.tick(delta);
}

void Client::render(ui::Frame &f) {
  bool gameUnderneath = false;
  if (shared.game) {
    shared.game->render(f);
    gameUnderneath = true;
  }

  const float gameFocusFactor = uiState.gameFocusFactor,
      pageFocusFactor = uiState.pageFocusFactor;
  if (!uiState.gameFocused()) {
    if (!gameUnderneath) {
      f.drawSprite(textureOf(Res::MenuBackground), {0, 0}, {0, 0, 1600, 900});
    } else {
      f.drawRect(
          {0, 0, 1600, 900},
          sf::Color(0, 0, 0,
                    (sf::Uint8) (linearTween(1, 0, gameFocusFactor) * 100)));
    }

    f.withAlpha(
        linearTween(1, 0, gameFocusFactor) *
        (gameUnderneath ? linearTween(0.5, 1, pageFocusFactor) : 1), [&]() {
      pageRects = {};
      drawPage(
          f, PageType::Home, style.homeOffset,
          "home", homePage);
      drawPage(
          f, PageType::Settings, style.listingOffset,
          "server listing", listingPage);
      drawPage(
          f, PageType::Listing,
          style.settingsOffset, "settings", settingsPage);
      f.withAlpha(pageFocusFactor, [&]() { backButton.render(f); });
    });
  }
}

bool Client::handle(const sf::Event &event) {
  if (uiState.gameFocused() and shared.game) {
    // first priority: if in-game, events are handled by the game
    return shared.game->handle(event);
  }

  if (event.type == sf::Event::KeyPressed
      and event.key.code == sf::Keyboard::Escape) {
    // second priority: escape key, the thing you press when you're lost
    if (uiState.pageFocused()) {
      referencePage(uiState.focusedPage).onLooseFocus();
      uiState.unfocusPage();
    } else if (shared.game) {
      shared.game->onFocus();
      uiState.focusGame();
    }
    return true;
  }

  if (uiState.pageFocused()) {
    // we're focused on a page

    if (backButton.handle(event)) {
      referencePage(uiState.focusedPage).onLooseFocus();
      backButton.reset();
      return true;
    }
    if (referencePage(uiState.focusedPage).handle(event)) return true;
  }

  if (uiState.menuFocused()) {
    // we're in the menu

    if (event.type == sf::Event::MouseButtonReleased) {
      const sf::Vector2f mouseClick =
          {(float) event.mouseButton.x, (float) event.mouseButton.y};

      for (auto &rect : pageRects)
        if (rect.first.contains(mouseClick)) uiState.focusPage(rect.second);
    }
  }

  return false;
}

void Client::signalRead() {
  if (shared.game) shared.game->signalRead();

  homePage.signalRead();
  listingPage.signalRead();
  settingsPage.signalRead();

  if (backButton.clickSignal) {
    unfocusPage();
  }
}

void Client::signalClear() {
  if (shared.game) shared.game->signalClear();
  homePage.signalClear();
  listingPage.signalClear();
  settingsPage.signalClear();

  backButton.signalClear();
}

void Client::beginGame(std::unique_ptr<Game> &&game) {
  uiState.focusGame();
  shared.gameFocused = true;
  shared.game = std::move(game);
}

void Client::focusGame() {
  uiState.focusGame();
  shared.gameFocused = true;
}

void Client::unfocusGame() {
  uiState.unfocusGame();
  shared.gameFocused = false;
}


void Client::unfocusPage() {
  referencePage(uiState.focusedPage).onLooseFocus();
  uiState.unfocusPage();
}

void Client::focusPage(const PageType type) {
  referencePage(type).onFocus();
  uiState.focusPage(type);
}

int main() {
  ui::runSFML([]() { return std::make_unique<Client>(); });
}

