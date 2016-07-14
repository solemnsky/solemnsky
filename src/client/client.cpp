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

/**
 * Client.
 */


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
  throw enum_error();
}

void Client::drawPage(ui::Frame &f, const PageType type,
                      const sf::Vector2f &offset, const std::string &name,
                      ui::Control &page) {
  const float focusFactor = uiState.pageFocusFactor;

  float alpha, scale, offsetAmnt, titleAlpha;
  if (type == uiState.focusedPage) {
    alpha = 1;
    scale = linearTween(style.menu.unfocusedPageScale, 1.0f, focusFactor);
    offsetAmnt = linearTween(1.0f, 0.0f, focusFactor);
    titleAlpha = linearTween(1.0f, 0.0f, focusFactor);
  } else {
    alpha = linearTween(1.0f, 0.0f, focusFactor);
    scale = style.menu.unfocusedPageScale;
    offsetAmnt = 1;
    titleAlpha = 1;
  }

  if (alpha == 0) return;

  const sf::Transform transform = sf::Transform()
      .translate(offsetAmnt * offset).scale(scale, scale);

  f.withAlpha(alpha, [&]() {
    f.withAlpha(titleAlpha, [&]() {
      f.drawText(
          sf::Vector2f(0, style.menu.pageDescMargin) + offsetAmnt * offset,
          name, sf::Color::White, style.menu.menuDescText,
          resources.defaultFont);
    });
    f.withTransform(transform, [&]() {
      f.drawRect({0, 0, 1600, 900}, style.menu.pageUnderlayColor);
      page.render(f);
    });
  });
}

void Client::drawUI(ui::Frame &f) {
  const Clamped &pageFocusFactor = uiState.pageFocusFactor;

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

  if (const auto game = shared.getGame()) {
    f.drawText(
        {style.menu.closeButtonOffset.x - 10, 0}, [&](ui::TextFrame &tf) {
          tf.setColor(sf::Color::White);
          tf.print(game->name);
          tf.setColor(style.menu.statusFontColor);
          tf.print("(" + game->status + ")");
        }, style.menu.gameDescText, resources.defaultFont);
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
  game->render(f);

  if (settings.enableDebug) {
    f.withAlpha(style.base.debugOpacity, [&]() {
      f.drawText(
          {20, 20},
          [&](ui::TextFrame &tf) {
            tf.setColor(sf::Color::Red);
            tf.printLn("PERFORMANCE STATS:");
            tf.setColor(sf::Color::White);
            tf.printLn("cycle:" + profilerSnap.cycleTime.print());
            tf.printLn("logic:" + profilerSnap.logicTime.print());
            tf.printLn("render:" + profilerSnap.renderTime.print());
            tf.setColor(sf::Color::Red);
            tf.breakLine();
            tf.printLn("GAME INFO:");
            tf.setColor(sf::Color::White);
            game->printDebugLeft(tf);
          }, style.base.debugText, resources.defaultFont);
      f.drawText(
          {1580, 20},
          [&](ui::TextFrame &tf) {
            tf.setColor(255, 0, 0);
            tf.printLn("MORE GAME INFO:");
            tf.setColor(255, 255, 255);
            tf.setColor(sf::Color::White);
            game->printDebugRight(tf);
          }, style.base.debugRightText, resources.defaultFont);
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
    homePage(shared),
    listingPage(shared),
    settingsPage(shared),
    tryingToQuit(false),

    profilerCooldown(1) {
  settings.readFromFile(Settings::saveFile);
  areChildren({&quitButton, &aboutButton, &closeButton, &backButton,
               &homePage, &listingPage, &settingsPage});
}

bool Client::poll() {
  if (game) {
    while (!game->poll()) {};
  }
  return ui::Control::poll();
}

void Client::tick(const float delta) {
  ui::Control::tick(delta);

  if (profilerCooldown.cool(delta)) {
    profilerSnap = ui::ProfilerSnapshot(references.profiler);
    profilerCooldown.reset();
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
      game.reset();
      blurGame();
    }
  }
}

void Client::render(ui::Frame &f) {
  const Clamped &gameFocusFactor = uiState.gameFocusFactor,
      &pageFocusFactor = uiState.pageFocusFactor;

  if (uiState.gameFocused() && game) {
    drawGame(f);
  } else {
    if (!game) {
      f.drawSprite(resources.getTexture(ui::TextureID::MenuBackground),
                   {0, 0}, {0, 0, 1600, 900});
    } else {
      drawGame(f);
      f.drawRect(
          {0, 0, 1600, 900},
          sf::Color(0, 0, 0,
                    (sf::Uint8) (linearTween(1, 0, gameFocusFactor) * 100)));
    }

    f.withAlpha(
        linearTween(1, 0, gameFocusFactor) *
            (game ? linearTween(style.menu.menuInGameFade, 1,
                                pageFocusFactor) : 1),
        [&]() { drawUI(f); });
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
    if (referencePage(uiState.focusedPage).handle(event)) return true;
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
    appLog("about message");
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
  else this->game = std::move(game);
  focusGame();
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
  }
}

void Client::blurPage() {
  referencePage(uiState.focusedPage).reset();
  backButton.reset();
  referencePage(uiState.focusedPage).onBlur();
  uiState.pageFocusing = false;
}

void Client::changeSettings(const SettingsDelta &settingsDelta) {
  settingsDelta.apply(settings);
  forAllPages([&](Page &page) {
    page.onChangeSettings(settingsDelta);
  });
  if (game) game->onChangeSettings(settingsDelta);
}
