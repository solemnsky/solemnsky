/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "client.hpp"
#include "elements/style.hpp"
#include "util/methods.hpp"
#include "util/printer.hpp"
#include "util/clientutil.hpp"

/**
 * Client.
 */

void Client::forAllPages(std::function<void(Page &)> f) {
  f(homePage.ctrl);
  f(listingPage.ctrl);
  f(settingsPage.ctrl);
}

ui::TransformedBase &Client::referencePageBase(const PageType type) {
  switch (type) {
    case PageType::Home:
      return homePage.base;
    case PageType::Settings:
      return settingsPage.base;
    case PageType::Listing:
      return listingPage.base;
  }
  throw enum_error();
}

Page &Client::referencePage(const PageType type) {
  switch (type) {
    case PageType::Home:
      return homePage.ctrl;
    case PageType::Settings:
      return settingsPage.ctrl;
    case PageType::Listing:
      return listingPage.ctrl;
  }
  throw enum_error();
}

void Client::drawDebugLeft(Printer &p) {
  p.setColor(255, 0, 0);
  p.printLn("PERFORMANCE STATS:");
  p.setColor(0, 0, 0);
  p.printLn("cycle:" + profilerSnap.cycleTime.print());
  p.printLn("logic:" + profilerSnap.logicTime.print());
  p.printLn("render:" + profilerSnap.renderTime.print());
  p.breakLine();
  p.setColor(255, 0, 0);
  p.printLn("GAME INFO:");
  p.setColor(0, 0, 0);
  game->printDebugLeft(p);
  p.breakLine();
}

void Client::drawDebugRight(Printer &p) {
  p.setColor(255, 0, 0);
  p.printLn("MORE GAME INFO:");
  p.setColor(0, 0, 0);
  game->printDebugRight(p);
  p.breakLine();
}

void Client::renderPage(ui::Frame &f, const PageType type,
                        const sf::Vector2f &offset, const std::string &name,
                        ui::TransformedBase &page) {
  const float focusFactor = uiState.pageFocusFactor;

  float alpha, scale, offsetAmnt, titleAlpha, backdropAlpha;
  if (type == uiState.focusedPage) {
    alpha = 1;
    scale = linearTween(style.menu.unfocusedPageScale, 1.0f, focusFactor);
    offsetAmnt = linearTween(1.0f, 0.0f, focusFactor);
    titleAlpha = linearTween(1.0f, 0.0f, focusFactor);
    backdropAlpha = 0;
  } else {
    alpha = linearTween(1.0f, 0.0f, focusFactor);
    scale = style.menu.unfocusedPageScale;
    offsetAmnt = 1;
    titleAlpha = 1;
    backdropAlpha = alpha;
  }

  if (alpha == 0) return;

  page.setTransform(sf::Transform()
                        .translate(offsetAmnt * offset)
                        .translate(0, linearTween(0, style.menu.pageYOffset, focusFactor))
                        .scale(scale, scale));

  f.withAlpha(alpha, [&]() {
    // Underlay
    f.withAlpha(backdropAlpha, [&]() {
      f.drawRect({0, 0, 1600, 900},
                 style.menu.pageUnderlayColor);
    });

    // Page itself.
    page.render(f);

    // Page title.
    f.withAlpha(titleAlpha, [&]() {
      f.drawText(
          sf::Vector2f(0, style.menu.pageDescMargin) + offsetAmnt * offset,
          name, style.base.freeTextColor, style.menu.menuDescText,
          resources.defaultFont);
    });
  });
}

void Client::renderUI(ui::Frame &f) {
  const Clamped &pageFocusFactor = uiState.pageFocusFactor;

  renderPage(
      f, PageType::Home, style.menu.homeOffset,
      "home", homePage.base);
  renderPage(
      f, PageType::Listing, style.menu.listingOffset,
      "server listing", listingPage.base);
  renderPage(
      f, PageType::Settings, style.menu.settingsOffset,
      "settings", settingsPage.base);

  if (const auto game = shared.getGame()) {
    f.drawText(
        style.menu.closeButtonOffset - sf::Vector2f(10, 0),
        [&](ui::TextFrame &tf) {
          tf.setColor(style.menu.statusFontColor);
          tf.print(" (" + game->status + ")");
          tf.print(game->name);
        }, style.menu.gameDescText, resources.defaultFont);
    closeButton.render(f);
  } else {
    if (!quitDisplayTimer) {
      f.drawText(
          style.menu.closeButtonOffset - sf::Vector2f(10, 0),
          [&](ui::TextFrame &tf) {
            tf.setColor(style.menu.statusFontColor);
            tf.print("(" + quittingReason + ")");
          }, style.menu.gameDescText, resources.defaultFont);
    }
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

void Client::renderGame(ui::Frame &f) {
  game->render(f);

  if (settings.enableDebug) {
    f.withAlpha(style.base.debugOpacity, [&]() {
      f.drawRect({0, 0}, {style.base.debugMargin, style.base.debugHeight}, style.base.debugBackground);
      f.drawText(
          {20, 20}, [&](ui::TextFrame &tf) { drawDebugLeft(tf); },
          style.base.debugText, resources.defaultFont);
      f.drawRect({1600, 0}, {1600 - style.base.debugMargin, style.base.debugHeight}, style.base.debugBackground);
      f.drawText(
          {1580, 20}, [&](ui::TextFrame &tf) { drawDebugRight(tf); },
          style.base.debugRightText, resources.defaultFont);
    });
  }
}

Client::Client(const ui::AppRefs &references) :
    ui::Control(references),
    backButton(references,
               style.menu.lowButtonStyle,
               style.menu.backButtonOffset,
               style.menu.backButtonText),
    closeButton(references,
                style.menu.lowButtonStyle,
                style.menu.closeButtonOffset,
                style.menu.closeButtonText),
    quitButton(references,
               style.menu.highButtonStyle,
               style.menu.quitButtonOffset,
               style.menu.quitButtonText),
    aboutButton(references,
                style.menu.highButtonStyle,
                style.menu.aboutButtonOffset,
                style.menu.aboutButtonText),

    shared(*this, references),
    homePage(std::make_unique<HomePage>(shared), sf::Transform()),
    listingPage(std::make_unique<ListingPage>(shared), sf::Transform()),
    settingsPage(std::make_unique<SettingsPage>(shared), sf::Transform()),
    tryingToQuit(false),
    quitDisplayTimer(style.menu.quitDescriptionFade),

    profileScheduler(1) {
  quitDisplayTimer.prime();
  areChildren({&quitButton, &aboutButton, &closeButton, &backButton,
               &homePage, &listingPage, &settingsPage});
}

void Client::poll() {
  if (game) game->poll();
  ui::Control::poll();
}

void Client::tick(const float delta) {
  ui::Control::tick(delta);

  quitDisplayTimer.tick(delta);

  if (profileScheduler.tick(delta)) {
    profilerSnap = ui::ProfilerSnapshot(references.profiler);
    profileScheduler.reset();
  }

  uiState.pageFocusFactor += style.menu.pageFocusAnimSpeed
      * delta * (uiState.pageFocusing ? 1 : -1);
  uiState.gameFocusFactor += style.menu.gameFocusAnimSpeed
      * delta * (uiState.gameFocusing ? 1 : -1);

  if (game) {
    game->tick(delta);
    if (game->quitting) {
      if (tryingToQuit) {
        quitting = true;
        return;
      }
      quittingReason = game->getQuittingReason();
      appLog("** Exited " + game->name + ". (" + quittingReason + ") **", LogOrigin::Client);
      quitDisplayTimer.reset();
      game.reset();
      blurGame();
    }
  }
}

void Client::render(ui::Frame &f) {
  const Clamped &gameFocusFactor = uiState.gameFocusFactor,
      &pageFocusFactor = uiState.pageFocusFactor;

  if (uiState.gameFocused() && game) {
    renderGame(f);
  } else {
    if (!game) {
      f.drawSprite(resources.getTexture(ui::TextureID::MenuBackground),
                   {0, 0}, {0, 0, 1600, 900});
    } else {
      renderGame(f);
      f.drawRect(
          {0, 0, 1600, 900},
          mixColors(style.menu.gameOverlayColor, sf::Color(255, 255, 255, 0), gameFocusFactor));
    }

    f.withAlpha(
        linearTween(1, 0, gameFocusFactor) *
            (game ? linearTween(style.menu.menuInGameFade, 1,
                                pageFocusFactor)
                  : linearTween(style.menu.menuNormalFade, 1, pageFocusFactor)),
        [&]() { renderUI(f); });
  }
}

bool Client::handle(const sf::Event &event) {
  if (game) {
    if (uiState.gameFocused()) {
      if (game->handle(event)) return true;
    } else {
      if (closeButton.handle(event)) return true;
    }
  }

  if (uiState.pageFocused()) {
    if (backButton.handle(event)) return true;
    if (referencePageBase(uiState.focusedPage).handle(event)) return true;
  }

  if (event.type == sf::Event::KeyPressed
      and event.key.code == sf::Keyboard::Escape) {
    // the escape key is pressed
    if (game) {
      if (uiState.gameFocused()) blurGame();
      else focusGame();
    } else blurPage();
    return false;
  }

  if (uiState.menuFocused()) {
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
  if (game) game->reset();
  referencePage(uiState.focusedPage).reset();
}

void Client::signalRead() {
  ui::Control::signalRead();
  if (game) game->signalRead();

  if (backButton.clickSignal) {
    blurPage();
  }

  if (quitButton.clickSignal) {
    if (game) {
      tryingToQuit = true;
      exitGame();
    }
    else quitting = true;
  }

  if (aboutButton.clickSignal) {
    appLog("<about message>");
  }

  if (closeButton.clickSignal) {
    exitGame();
  }
}

void Client::signalClear() {
  if (game) game->signalClear();
  ui::Control::signalClear();
}

void Client::beginGame(std::unique_ptr<Game> &&game) {
  if (this->game) exitGame();
  else {
    this->game = std::move(game);
    appLog("** Started " + this->game->name + ". **", LogOrigin::Client);
    focusGame();
  }
}

void Client::focusGame() {
  if (game) {
    reset();
    if (uiState.pageFocused()) referencePage(uiState.focusedPage).onBlur();
    uiState.gameFocusing = true;
    game->onFocus();
  }
}

void Client::blurGame() {
  uiState.gameFocusing = false;
  if (game) {
    game->reset();
    game->onBlur();
  }
}

void Client::exitGame() {
  if (game) game->doExit();
}

void Client::focusPage(const PageType page) {
  if (uiState.pageFocusFactor != 0) {
    uiState.pageFocusing = false;
  } else {
    uiState.pageFocusing = true;
    uiState.focusedPage = page;
    referencePage(page).onFocus();
  }
}

void Client::blurPage() {
  referencePage(uiState.focusedPage).reset();
  backButton.reset();
  referencePage(uiState.focusedPage).onBlur();
  uiState.pageFocusing = false;
}

void Client::changeSettings(const ui::SettingsDelta &settingsDelta) {
  settingsDelta.apply(settings);
  forAllPages([&](Page &page) {
    page.onChangeSettings(settingsDelta);
  });
  if (game) game->onChangeSettings(settingsDelta);
}
