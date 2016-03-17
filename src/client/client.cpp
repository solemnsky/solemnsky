#include "client.h"
#include "elements/style.h"
#include "util/methods.h"

/**
 * Client.
 */

Client::Client() :
    quitButton(style.menu.highButtonStyle,
               style.menu.quitButtonOffset,
               style.menu.quitButtonText),
    aboutButton(style.menu.highButtonStyle,
                style.menu.aboutButtonOffset,
                style.menu.aboutButtonText),
    closeButton(style.menu.lowButtonStyle,
                style.menu.closeButtonOffset,
                style.menu.closeButtonText),
    backButton(style.menu.lowButtonStyle,
               style.menu.backButtonOffset,
               style.menu.backButtonText),

    shared(this),
    homePage(shared),
    listingPage(shared),
    settingsPage(shared),
    tryingToQuit(false) {
  areChildren({&quitButton, &aboutButton, &closeButton, &backButton,
               &homePage, &listingPage, &settingsPage});
}

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
    scale = linearTween(style.menu.unfocusedPageScale, 1,
                        focusFactor);
    offsetAmnt = linearTween(1, 0, focusFactor);
    titleAlpha = linearTween(1, 0, focusFactor);
  } else {
    alpha = linearTween(1, 0, focusFactor);
    scale = style.menu.unfocusedPageScale;
    offsetAmnt = 1;
    titleAlpha = 1;
  }

  if (alpha == 0) return;

  sf::Transform &transform =
      sf::Transform().translate(offsetAmnt * offset).scale(scale, scale);

  f.withAlpha(alpha, [&]() {
    f.withAlpha(titleAlpha, [&]() {
      f.drawText(
          sf::Vector2f(
              0, style.menu.pageDescMargin - style.menu.descSize)
              + offsetAmnt * offset,
          {name},
          style.menu.descSize);
    });
    f.withTransform(transform, [&]() {
      f.drawRect({0, 0, 1600, 900}, style.menu.pageUnderlayColor);
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
  ui::Control::tick(delta);

  shared.uptime += delta;
  shared.ui.tick(delta);

  if (shared.game) {
    shared.game->tick(delta);
    if (shared.game->quitting) {
      if (tryingToQuit) {
        quitting = true;
        return;
      }
      shared.game.reset();
      shared.ui.blurGame();
    }
  }
}

void Client::render(ui::Frame &f) {
  if (shared.settings.enableDebug) {
    const float cycleTime =
        shared.appState->profiler.logicTime.average() +
            shared.appState->profiler.renderTime.average();

    const float actualCycleTime = shared.appState->profiler.cycleTime.average();

    // TODO: cute debug HUD
  }

  bool gameUnderneath = (bool) shared.game;
  const float gameFocusFactor = shared.ui.gameFocusFactor,
      pageFocusFactor = shared.ui.pageFocusFactor;
  // gameFocusFactor: 0 means tutorial is blurred, 1 means the tutorial is focused
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
            (gameUnderneath ? linearTween(style.menu.menuInGameFade, 1,
                                          pageFocusFactor) :
             1),
        [&]() {
          pageRects = {}; // populated by drawPage(), used as the click rects
          // for event handling (TODO: remove if the actually useful click rects
          // are always static provided we only allow clicking on pages when
          // they're completely blurred)

          drawPage(
              f, PageType::Home, style.menu.homeOffset,
              "home", homePage);
          drawPage(
              f, PageType::Listing, style.menu.listingOffset,
              "server listing", listingPage);
          drawPage(
              f, PageType::Settings,
              style.menu.settingsOffset, "settings", settingsPage);

          if (gameUnderneath) {
            const std::string statusStr = "(" + shared.game->status + ") ";
            const float descLength = f.textSize(shared.game->name,
                                                style.menu.descSize).x,
                statusLength = f.textSize(statusStr,
                                          style.menu.descSize).x;
            f.drawText(
                {style.menu.closeButtonOffset.x - descLength - statusLength -
                    10, 0},
                {shared.game->name}, style.menu.descSize);
            f.drawText(
                {style.menu.closeButtonOffset.x - statusLength, 0},
                {statusStr}, style.menu.descSize,
                style.menu.statusFontColor);
            closeButton.render(f);
          }
          f.withAlpha(
              linearTween(1, 0, pageFocusFactor),
              [&]() {
                // buttons that fade out as the page focuses
                quitButton.render(f);
                aboutButton.render(f);
              });
          f.withAlpha(
              linearTween(0, 1, pageFocusFactor),
              [&]() {
                // buttons that fade in as the page focuses
                backButton.render(f);
              });
        });
  }
}

bool Client::handle(const sf::Event &event) {
  if (shared.ui.gameFocused() and shared.game) {
    if (shared.game->handle(event)) return true;
  }

  if (shared.game and (shared.ui.menuFocused() or shared.ui.pageFocused())) {
    if (closeButton.handle(event)) return true;
  }

  if (shared.ui.pageFocused()) {
    if (backButton.handle(event)) return true;
    if (referencePage(shared.ui.focusedPage).handle(event)) return true;
  }

  if (event.type == sf::Event::KeyPressed
      and event.key.code == sf::Keyboard::Escape) {
    // the escape key is pressed
    if (shared.game) {
      if (shared.ui.gameFocused()) blurGame();
      else focusGame();
    } else blurPage();
    return true;
  }

  if (shared.ui.menuFocused()) {
    // we're in the menu

    if (quitButton.handle(event)) return true;
    if (aboutButton.handle(event)) return true;

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

void Client::reset() {
  ui::Control::reset();
}

void Client::signalRead() {
  ui::Control::signalRead();
  if (shared.game) shared.game->signalRead();

  if (backButton.clickSignal) {
    blurPage();
  }

  if (quitButton.clickSignal) {
    if (shared.game) {
      tryingToQuit = true;
      exitGame();
    }
    else quitting = true;
  }

  if (aboutButton.clickSignal) {
    appLog("about message");
  }

  if (closeButton.clickSignal) {
    exitGame();
  }
}

void Client::signalClear() {
  if (shared.game) shared.game->signalClear();
  ui::Control::signalClear();
}

void Client::beginGame(std::unique_ptr<Game> &&game) {
  shared.ui.focusGame();
  shared.game = std::move(game);
  focusGame();
}

void Client::focusGame() {
  if (shared.game) {
    reset();
    if (shared.ui.pageFocused()) referencePage(shared.ui.focusedPage).onBlur();
    shared.ui.focusGame();
    shared.game->onFocus();
  }
}

void Client::blurGame() {
  shared.ui.blurGame();
  if (shared.game) shared.game->onBlur();
}

void Client::exitGame() {
  if (shared.game) shared.game->doExit();
}

void Client::focusPage(const PageType type) {
  shared.ui.focusPage(type);
}

void Client::blurPage() {
  shared.ui.blurPage();
}

void Client::changeSettings(const SettingsDelta &settings) {
  settings.apply(shared.settings);
  forAllPages([&settings](Page &page) {
    page.onChangeSettings(settings);
  });
  if (shared.game) shared.game->onChangeSettings(settings);
}

int main() {
  // and He said,
  ui::runSFML([]() { return std::make_unique<Client>(); });
  // and lo, there appeared a client
}
