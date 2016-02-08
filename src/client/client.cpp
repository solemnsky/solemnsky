#include "client.h"
#include "util/methods.h"

/**
 * ClientUIState.
 */

ClientUIState::ClientUIState() :
    focusedPage(PageType::Home),
    pageFocusing(false),
    pageFocusAnim(0, 1, 0),
    gameFocusAnim(0, 1, 0) { }

float ClientUIState::getPageFocusFactor() { return pageFocusAnim; }

float ClientUIState::getGameFocusFactor() { return gameFocusAnim; }

bool ClientUIState::getPageFocused() { return pageFocusAnim == 1; }

bool ClientUIState::getGameFocused() { return gameFocusAnim == 1; }

PageType ClientUIState::getPageType() { return focusedPage; }

void ClientUIState::focusGame() {

}

void ClientUIState::focusPage(PageType type) {

}

void ClientUIState::unfocusPage() {

}

void ClientUIState::tick(float delta) {

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
  float alpha, scale, offsetAmnt, titleAlpha;

  if (type == shared.focusedPage) {
    alpha = 1;
    scale = linearTween(unfocusedPageScale, 1, focusAnim);
    offsetAmnt = linearTween(1, 0, focusAnim);
    titleAlpha = linearTween(1, 0, focusAnim);
  } else {
    alpha = linearTween(1, 0, focusAnim);
    scale = unfocusedPageScale;
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
    if (shared.game->concluded) shared.game = nullptr;
  }

  focusAnim += delta * pageFocusAnimSpeed * (pageFocused ? 1 : -1);
  gameFocusAnim += delta * gameFocusAnimSpeed * (shared.gameInFocus() ? 1 : -1);

  if (focusAnim == 0) backButton.reset();

  forAllPages([&delta](Page &page) { page.tick(delta); });
  backButton.tick(delta);
}

void Client::render(ui::Frame &f) {
  bool gameUnderneath = false;
  if (shared.game) {
    shared.game->render(f);
    gameUnderneath = true;
  }

  if (gameFocusAnim != 1) {
    if (!gameUnderneath) {
      f.drawSprite(textureOf(Res::MenuBackground), {0, 0}, {0, 0, 1600, 900});
    } else {
      f.drawRect(
          {0, 0, 1600, 900},
          sf::Color(0, 0, 0,
                    (sf::Uint8) (linearTween(1, 0, gameFocusAnim) * 100)));
    }

    f.withAlpha(
        linearTween(1, 0, gameFocusAnim) *
        (gameUnderneath ? linearTween(0.5, 1, focusAnim) : 1), [&]() {
      pageRects = {};
      drawPage(f, PageType::Home, homeOffset, "home",
               homePage);
      drawPage(f, PageType::Settings, listingOffset, "server listing",
               listingPage);
      drawPage(f, PageType::Listing, settingsOffset, "settings",
               settingsPage);
      f.withAlpha(focusAnim, [&]() { backButton.render(f); });
    });
  }
}

bool Client::handle(const sf::Event &event) {
  if (shared.gameInFocus()) {
    return shared.game->handle(event);
  } else {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape) {

        // escape key, the thing you press when you're totally lost
        // let's make it as helpful as possible, shall we?
        if (pageFocused) {
          pageFocused = false;
          referencePage(focusedPage).onLooseFocus();
        } else {
          if (shared.game) shared.game->inFocus = true;
          referencePage(focusedPage).onLooseFocus();
        }
        return true;
      }
    }

    if (focusAnim > 0) if (backButton.handle(event)) return true;

    if (focusAnim == 1) {
      // we're totally focused on a certain control
      referencePage(focusedPage).handle(event);
    } else {
      // we're still in the control menu
      if (event.type == sf::Event::MouseButtonReleased) {
        const sf::Vector2f mouseClick =
            {(float) event.mouseButton.x, (float) event.mouseButton.y};

        pageFocused = false;
        for (auto &rect : pageRects) {
          if (rect.first.contains(mouseClick)) {
            pageFocused = true;
            focusedPage = rect.second;
          }
        }
      }
    }
  }
  return true;
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


void Client::unfocusPage() {
  pageFocused = false;
  referencePage(focusedPage).onLooseFocus();
}

void Client::focusPage(const PageType type) {
  focusedPage = type;
  pageFocusing = true;
}

void Client::beginGame(std::unique_ptr<Game> &&game) {
  unfocusPage();
  shared.game = std::move(game);
  shared.game->inFocus = true;
}

int main() {
  ui::runSFML([]() { return std::make_unique<Client>(); });
}

