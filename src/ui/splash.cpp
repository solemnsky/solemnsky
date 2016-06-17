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
#include "splash.hpp"
#include "client/elements/style.hpp"

namespace ui {

namespace detail {

void SplashScreen::constructApp(const AppResources &resources) {
  initializedReferences.emplace(
      resources,
      references.uptime,
      references.window,
      references.profiler);
  animBegin = references.uptime;
  control = mkApp(initializedReferences.get());
  areChildren({control.get()});
}

SplashScreen::SplashScreen(
    const AppRefs &references,
    std::function<std::unique_ptr<Control>(const AppRefs &)> mkApp) :
    Control(references),
    mkApp(mkApp),
    loader(
        {FontID::Default},
        {TextureID::MenuBackground}),
    defaultFont(loader.accessFont(FontID::Default)),
    background(loader.accessTexture(ui::TextureID::MenuBackground)),
    animBegin(references.uptime) {
  loader.loadAllThreaded();
}

bool SplashScreen::poll() {
  ConsolePrinter printer{LogOrigin::App};

  if (control) quitting = control->quitting;

  return Control::poll();
}

void SplashScreen::tick(const TimeDiff delta) {
  Control::tick(delta);
}

void SplashScreen::render(ui::Frame &f) {
  if (!control) {
    f.drawSprite(background, {}, {0, 0, 1600, 900});
    if (!loader.getHolder()) {
      f.drawText({800, 450}, "loading resources...",
                 sf::Color::White, style.splash.titleFormat,
                 defaultFont);
      f.drawRect({800.0f - (style.splash.barWidth / 2.0f),
                  (450.0f + style.splash.barPaddingTop),
                  (loader.getProgress() * style.splash.barWidth),
                  style.splash.barHeight},
                 style.splash.barColor);
    } else {
      f.withAlpha(
          linearTween(0.3, 1, sineAnim(float(references.uptime), 0.2)),
          [&]() {
            f.drawText({800, 450}, "press any key to begin",
                       sf::Color::White, style.splash.titleFormat,
                       defaultFont);
          });
    }
  } else {
    const float animTime = float(references.timeSince(animBegin));
    if (animTime < 0.5) {
      f.drawSprite(loader.accessTexture(ui::TextureID::MenuBackground),
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
      constructApp(*loader.getHolder());
      return true;
    }
  }

  return ui::Control::handle(event);
}

}

}

