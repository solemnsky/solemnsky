#include "client.h"
#include "util/methods.h"

/**
 * Client.
 */
ui::Button::Style Client::Style::backButtonStyle() const {
  ui::Button::Style style;
  style.fontSize = 50;
  return style;
}

ui::Button::Style Client::Style::quitButtonStyle() const {
  ui::Button::Style style;
  style.fontSize = 50;
  return style;
}

Client::Client() :
    backButton({1400, 800}, style.backButtonText, style.backButtonStyle()),
    quitButton({200, 100}, "", style.quitButtonStyle()),

    shared(this),
    homePage(shared),
    listingPage(shared),
    settingsPage(shared) { }

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
    if (shared.game->quitting) {
      shared.game.reset();
      shared.ui.unfocusGame();
    }
  }

  forAllPages([&delta](Page &page) { page.tick(delta); });
  backButton.tick(delta);
  quitButton.tick(delta);
}

void Client::render(ui::Frame &f) {
  bool gameUnderneath = (bool) shared.game;
  const float gameFocusFactor = shared.ui.gameFocusFactor,
      pageFocusFactor = shared.ui.pageFocusFactor;
  // gameFocusFactor: 0 means game is blurred, 1 means the game is focused
  // pageFocusFactor: 0 means we're in the menu, 1 means a page focused

  if (shared.ui.gameFocused()) {
    if (shared.game) shared.game->render(f);
  } else {
    if (!gameUnderneath) {
      f.drawSprite(textureOf(Res::MenuBackground), {0, 0}, {0, 0, 1600, 900});
    } else {
      shared.game->render(f);
      f.drawRect(
          {0, 0, 1600, 900},
          sf::Color(0, 0, 0,
                    (sf::Uint8) (linearTween(1, 0, gameFocusFactor) * 100)));
    }

    f.withAlpha(
        linearTween(1, 0, gameFocusFactor) *
            (gameUnderneath ? linearTween(0.5, 1, pageFocusFactor) : 1),
        [&]() {
          pageRects = {}; // populated by drawPage(), used as the click rects
          // for event handling (TODO: potentially remove, if the actually
          // useful click rects are always static, since we only allow
          // clicking on pages when they're completely blurred)

          drawPage(
              f, PageType::Home, style.homeOffset,
              "home", homePage);
          drawPage(
              f, PageType::Listing, style.listingOffset,
              "server listing", listingPage);
          drawPage(
              f, PageType::Settings,
              style.settingsOffset, "settings", settingsPage);

          quitButton.text = gameUnderneath ? style.quitGameText :
                            style.quitAppText;
          f.withAlpha(linearTween(1, 0, pageFocusFactor),
                      [&]() { quitButton.render(f); });
          f.withAlpha(linearTween(0, 1, pageFocusFactor),
                      [&]() { backButton.render(f); });
        });
  }
}

bool Client::handle(const sf::Event &event) {
  if (shared.ui.gameFocused() and shared.game) {
    // events handled by the game
    if (shared.game->handle(event)) return true;
  }

  if (shared.ui.pageFocused()) {
    // events handled by the back button / focused page
    if (backButton.handle(event)) return true;
    if (referencePage(shared.ui.focusedPage).handle(event)) return true;
  }

  if (event.type == sf::Event::KeyPressed
      and event.key.code == sf::Keyboard::Escape) {
    // the escape key, unfocuses pages / focuses the game
    if (!shared.ui.menuFocused()) {
      unfocusPage();
    } else focusGame();
    return true;
  }

  if (shared.ui.menuFocused()) {
    // quit button is pressed
    if (quitButton.handle(event)) return true;

    // pages are clicked in the menu
    if (event.type == sf::Event::MouseButtonReleased) {
      const sf::Vector2f mouseClick =
          {(float) event.mouseButton.x, (float) event.mouseButton.y};

      for (auto &rect : pageRects)
        if (rect.first.contains(mouseClick)) {
          focusPage(rect.second);
          return true;
        }

      focusGame(); // clicked somewhere besides the pages
      return true;
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

  if (quitButton.clickSignal) {
    if (shared.game) exitGame();
    else quitting = true;
  }
}

void Client::signalClear() {
  if (shared.game) shared.game->signalClear();
  homePage.signalClear();
  listingPage.signalClear();
  settingsPage.signalClear();

  backButton.signalClear();
  quitButton.signalClear();
}

void Client::beginGame(std::unique_ptr<Game> &&game) {
  shared.ui.focusGame();
  shared.game = std::move(game);
  focusGame();
}

void Client::exitGame() {
  if (shared.game) shared.game->doExit();
}

void Client::focusGame() {
  if (shared.game) {
    unfocusPage();
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

void Client::changeSettings(const SettingsDelta &settings) {
  settings.apply(shared.settings);
  forAllPages([&settings](Page &page) {
    page.onChangeSettings(settings);
  });
  if (shared.game) shared.game->onChangeSettings(settings);
}

int main() {
  ui::runSFML([]() { return std::make_unique<Client>(); });
}

