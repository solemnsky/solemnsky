#include "client.h"
#include "elements/style.h"
#include "util/methods.h"
#include "util/printer.h"

/**
 * Client.
 */

Client::Client(ui::AppState &appState) :
    ui::Control(appState),
    backButton(appState,
               style.menu.lowButtonStyle,
               style.menu.backButtonOffset,
               style.menu.backButtonText),
    closeButton(appState,
                style.menu.lowButtonStyle,
                style.menu.closeButtonOffset,
                style.menu.closeButtonText),
    quitButton(appState,
               style.menu.highButtonStyle,
               style.menu.quitButtonOffset,
               style.menu.quitButtonText),
    aboutButton(appState,
                style.menu.highButtonStyle,
                style.menu.aboutButtonOffset,
                style.menu.aboutButtonText),

    shared(appState, *this),
    homePage(shared),
    listingPage(shared),
    settingsPage(shared),
    tryingToQuit(false),

    profilerCooldown(1) {
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
  return homePage;
}

void Client::tick(float delta) {
  ui::Control::tick(delta);

  if (profilerCooldown.cool(delta)) {
    profilerSnap = ui::ProfilerSnapshot(appState.profiler);
    profilerCooldown.reset();
  }

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
              0, style.menu.pageDescMargin) + offsetAmnt * offset,
          name, sf::Color::White, style.menu.menuDescText);
    });
    f.withTransform(transform, [&]() {
      f.drawRect({0, 0, 1600, 900}, style.menu.pageUnderlayColor);
      page.render(f);
    });
  });
}


void Client::drawUI(ui::Frame &f) {
  const Clamped &pageFocusFactor = shared.ui.pageFocusFactor;

  // draw the pages
  drawPage(
      f, PageType::Home, style.menu.homeOffset,
      "HOME", homePage);
  drawPage(
      f, PageType::Listing, style.menu.listingOffset,
      "SERVER LISTING", listingPage);
  drawPage(
      f, PageType::Settings, style.menu.settingsOffset,
      "SETTINGS", settingsPage);

  if (shared.game) {
    f.drawText(
        {style.menu.closeButtonOffset.x - 10, 0}, [&](ui::TextFrame &tf) {
          tf.setColor(sf::Color::White);
          tf.print(shared.game->name);
          tf.setColor(style.menu.statusFontColor);
          tf.print("(" + shared.game->status + ")");
        }, style.menu.gameDescText);
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
}

void Client::drawGame(ui::Frame &f) {
  shared.game->render(f);

  if (shared.settings.enableDebug) {
    f.drawText(
        {10, 10},
        [&](ui::TextFrame &tf) {
          tf.setColor(sf::Color::White);
          tf.printLn("cycle:" + profilerSnap.cycleTime.print());
          tf.printLn("logic:" + profilerSnap.logicTime.print());
          tf.printLn("render:" + profilerSnap.renderTime.print());
        }, style.base.normalText);
  }
}

void Client::render(ui::Frame &f) {
  const Clamped &gameFocusFactor = shared.ui.gameFocusFactor,
      &pageFocusFactor = shared.ui.pageFocusFactor;

  if (shared.ui.gameFocused() && shared.game) {
    drawGame(f);
  } else {
    if (!shared.game) {
      f.drawSprite(textureOf(ResID::MenuBackground), {0, 0}, {0, 0, 1600, 900});
    } else {
      drawGame(f);
      f.drawRect(
          {0, 0, 1600, 900},
          sf::Color(0, 0, 0,
                    (sf::Uint8) (linearTween(1, 0, gameFocusFactor) * 100)));
    }

    f.withAlpha(
        linearTween(1, 0, gameFocusFactor) *
            (shared.game ? linearTween(style.menu.menuInGameFade, 1,
                                       pageFocusFactor) : 1),
        [&]() { drawUI(f); });
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

      for (const std::pair<sf::FloatRect, PageType> &rect : {
          std::pair<sf::FloatRect, PageType>(
              style.menu.homeArea, PageType::Home),
          std::pair<sf::FloatRect, PageType>(
              style.menu.settingsArea, PageType::Settings),
          std::pair<sf::FloatRect, PageType>(
              style.menu.listingArea, PageType::Listing)
      }) {
        if (rect.first.contains(mouseClick)) {
          focusPage(rect.second);
          return true;
        }
      }

      focusGame(); // clicked somewhere besides the pages
      return true;
    }
  }

  return false;
}

void Client::reset() {
  ui::Control::reset();
  if (shared.game) shared.game->reset();
  referencePage(shared.ui.focusedPage).reset();
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
  if (shared.game) exitGame();
  else shared.game = std::move(game);
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
  if (shared.game) {
    shared.game->reset();
    shared.game->onBlur();
  }
}

void Client::exitGame() {
  if (shared.game) shared.game->doExit();
}

void Client::focusPage(const PageType type) {
  shared.ui.focusPage(type);
}

void Client::blurPage() {
  referencePage(shared.ui.focusedPage).reset();
  backButton.reset();
  referencePage(shared.ui.focusedPage).onBlur();
  shared.ui.blurPage();
}

void Client::changeSettings(const SettingsDelta &settings) {
  settings.apply(shared.settings);
  forAllPages([&settings](Page &page) {
    page.onChangeSettings(settings);
  });
  if (shared.game) shared.game->onChangeSettings(settings);
}
