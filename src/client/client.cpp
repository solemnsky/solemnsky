#include "client.h"
#include "util/methods.h"

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
    backButton({1400, 800}, "go back", backButtonStyle()) { }

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
  const float focusFactor = shared.ui.pageFocusFactor;

  float alpha, scale, offsetAmnt, titleAlpha;
  if (type == shared.ui.focusedPage) {
    alpha = 1;
    scale = linearTween(style.unfocusedPageScale, 1,
                        focusFactor);
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
  shared.ui.tick(delta);

  if (shared.game) {
    shared.game->tick(delta);
    if (shared.game->concluded) {
      shared.game = nullptr;
      shared.ui.unfocusGame();
    }
  }

  forAllPages([&delta](Page &page) { page.tick(delta); });
  backButton.tick(delta);
}

void Client::render(ui::Frame &f) {
  bool gameUnderneath = false;
  if (shared.game) {
    shared.game->render(f);
    gameUnderneath = true;
  }

  const float gameFocusFactor = shared.ui.gameFocusFactor,
      pageFocusFactor = shared.ui.pageFocusFactor;
  if (!shared.ui.gameFocused()) {
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
  if (shared.ui.gameFocused() and shared.game) {
    // events handled by the game
    return shared.game->handle(event);
  }

  if (shared.ui.pageFocused()) {
    // events handled by the back button / focused page
    if (backButton.handle(event)) return true;
    if (referencePage(shared.ui.focusedPage).handle(event)) return true;
  }

  if (event.type == sf::Event::KeyPressed
      and event.key.code == sf::Keyboard::Escape) {
    // the escape key, unfocuses pages / focuses the game
    if (!shared.ui.gameFocused() and !shared.ui.menuFocused()) {
      referencePage(shared.ui.focusedPage).onLooseFocus();
      shared.ui.unfocusPage();
    } else if (shared.game) {
      shared.game->onFocus();
      shared.ui.focusGame();
    }
    return true;
  }

  if (shared.ui.menuFocused()) {
    // pages are clicked in the menu
    if (event.type == sf::Event::MouseButtonReleased) {
      const sf::Vector2f mouseClick =
          {(float) event.mouseButton.x, (float) event.mouseButton.y};

      for (auto &rect : pageRects)
        if (rect.first.contains(mouseClick)) focusPage(rect.second);
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
  shared.ui.focusGame();
  shared.game = std::move(game);
  shared.game->onFocus();
}

void Client::focusGame() {
  if (shared.game) {
    shared.ui.focusGame();
    shared.game->onFocus();
  }
}

void Client::unfocusGame() {
  shared.ui.unfocusGame();
  if (shared.game) shared.game->onLooseFocus();
}


void Client::unfocusPage() {
  referencePage(shared.ui.focusedPage).onLooseFocus();
  shared.ui.unfocusPage();
}

void Client::focusPage(const PageType type) {
  referencePage(type).onFocus();
  shared.ui.focusPage(type);
}

int main() {
  ui::runSFML([]() { return std::make_unique<Client>(); });
}

