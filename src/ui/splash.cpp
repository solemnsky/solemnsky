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
    AppState &appState, std::function< Control(const AppState &)
> mkApp) :
    Control(appState),
    loader(
        {FontID::Default},
        {TextureID::MenuBackground}),
    animBegin(appState.uptime),
    readyText(80, {}, ui::HorizontalAlign::Center, ui::VerticalAlign::Middle),
    mkApp(mkApp) {
  loader.loadAllThreaded();
}

bool SplashScreen::poll() {
  if (control) {
    quitting = control->quitting;
  } else {
    ConsolePrinter printer{LogOrigin::App};
    loader.printNewLogs(printer);
  }

  return Control::poll();
}

void SplashScreen::tick(const TimeDiff delta) {
  Control::tick(delta);
}

void SplashScreen::render(ui::Frame &f) {
  if (!control) {
    f.drawSprite(loader.accessTexture(ui::TextureID::MenuBackground),
                 {}, {0, 0, 1600, 900});
    if (!loader.getHolder()) {
      f.drawText({800, 450}, "loading resources...",
                 style.base.normalText, sf::Color::White,);
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
      f.drawSprite(textureOf(ui::TextureID::MenuBackground),
                   {}, {0, 0, 1600, 900});
      f.withAlpha(linearTween(0, 1, animTime * 2), [&]() {
        ui::Control::render(f);
      });
    } else {
      Control::render(f);
    }
  }
}

bool SplashScreen::handle(const sf::Event &event) {
  if (loader.getHolder() && !control) {
    if (event.type == sf::Event::KeyReleased
        or event.type == sf::Event::MouseButtonReleased) {
      appState.resources = loader.getHolder();
      control.emplace(mkApp(appState));
      animBegin = appState.uptime;
      areChildren({control.get_ptr()});
      return true;
    }
  }

  return ui::Control::handle(event);
}

}

}
