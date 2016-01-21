#include "client.h"

ui::Button::Style backButtonStyle() {
  return {};
}

Client::Client() :
    state(),
    homePage(&state),
    listingPage(&state),
    settingsPage(&state),
    focusedPage(PageType::Home),
    backButton({1500, 850}, "go back", backButtonStyle()),
    pageFocused(false),
    focusAnim(0, 1, 0) { }

ui::Control &Client::referencePage(const PageType type) {
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

  if (type == focusedPage) {
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
  state.appState = appState;
}

void Client::tick(float delta) {
  state.uptime += delta;

  if (state.game) {
    state.game->tick(delta);
  }

  focusAnim += delta * pageFocusAnimSpeed * (pageFocused ? 1 : -1);
  if (focusAnim == 0) backButton.reset();
  homePage.tick(delta);
  settingsPage.tick(delta);
  listingPage.tick(delta);
  backButton.tick(delta);
}

void Client::render(ui::Frame &f) {
  bool drawPages = true;
  bool gameUnderneath = false;

  if (state.game) {
    state.game->render(f);
    if (state.game->inFocus) {
      drawPages = false;
    } else {
      gameUnderneath = true;
    }
  }

  if (drawPages) {
    if (!gameUnderneath) {
      f.drawSprite(textureOf(Res::MenuBackground), {0, 0}, {0, 0, 1600, 900});
    }

    f.withAlpha(gameUnderneath ? 0.5f : 1, [&]() {
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

void Client::handle(const sf::Event &event) {
  if (state.game) {
    state.game->handle(event);
  } else {
    if (event.type == sf::Event::KeyPressed) {
      if (event.key.code == sf::Keyboard::Escape) {
        pageFocused = false;
        return;
      }
    }

    if (focusAnim > 0)
      backButton.handle(event);

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
}

void Client::signalRead() {
  if (!backButton.clickSignal.empty()) {
    pageFocused = false;
  }
}

void Client::signalClear() {
  backButton.signalClear();
}

