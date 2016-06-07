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
#include "splash.h"
#include "client/elements/style.h"

namespace ui {

namespace detail {

SplashScreen::SplashScreen(
    AppState &appState,
    std::function<Control(AppState &)>
    mkApp) :
    Control(appState),
    animBegin(appState.uptime),
    drewScreen(false), loadingDone(false),
    readyText(80, {},
              ui::HorizontalAlign::Center, ui::VerticalAlign::Middle,
              ui::FontID::Default),
    mkApp(mkApp) {
  loader.loadBoostrap(
      {FontID::Default},
      {TextureID::MenuBackground});
  loader.loadAllThreaded();
}

bool SplashScreen::poll() {
  if (!control && drewScreen && !loadingDone) {
    loadResources();
    loadingDone = true;
  }

  if (control) {
    quitting = control->quitting;
  }

  return ui::Control::poll();
}

void SplashScreen::tick(const TimeDiff delta) {
  ui::Control::tick(delta);
}

void SplashScreen::render(ui::Frame &f) {
  drewScreen = true;

  if (!control) {
    f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
    if (!loadingDone) {
      f.drawText({800, 450}, "loading resources...",
                 sf::Color::White, readyText);
    } else {
      f.withAlpha(
          linearTween(0.3, 1, sineAnim(float(appState.uptime), 0.2)),
          [&]() {
            f.drawText({800, 450}, "press any key to begin",
                       sf::Color::White, readyText);
          });
    }
  } else {
    const float animTime = float(appState.timeSince(animBegin));
    if (animTime < 0.5) {
      f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
      f.withAlpha(linearTween(0, 1, animTime * 2), [&]() {
        ui::Control::render(f);
      });
    } else {
      ui::Control::render(f);
    }
  }
}

bool SplashScreen::handle(const sf::Event &event) {
  if (loadingDone && !control) {
    if (event.type == sf::Event::KeyReleased
        or event.type == sf::Event::MouseButtonReleased) {
      control = std::move(mkApp(appState));
      animBegin = appState.uptime;
      areChildren({control.get()});
      return true;
    }
  }

  return ui::Control::handle(event);
}

}

}
