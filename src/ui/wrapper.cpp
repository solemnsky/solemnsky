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
#include "wrapper.h"
#include "client/elements/style.h"

namespace ui {

namespace detail {

ExecWrapper::ExecWrapper(
    AppState &appState,
    std::function<std::unique_ptr<Control>(AppState &)>
    mainCtrlCtor) :

    Control(appState),

    animBegin(appState.uptime),
    drewScreen(false), loadingDone(false),
    loadingText(80, {},
                ui::HorizontalAlign::Center, ui::VerticalAlign::Middle,
                ResID::Font),
    mainCtrlCtor(mainCtrlCtor) {
  loadSplashResources();
}

bool ExecWrapper::poll() {
  if (!mainCtrl && drewScreen && !loadingDone) {
    loadResources();
    loadingDone = true;
  }

  if (mainCtrl) {
    quitting = mainCtrl->quitting;
  }

  return ui::Control::poll();
}

void ExecWrapper::tick(const TimeDiff delta) {
  ui::Control::tick(delta);
}

void ExecWrapper::render(ui::Frame &f) {
  drewScreen = true;

  if (!mainCtrl) {
    f.drawSprite(textureOf(ResID::MenuBackground), {}, {0, 0, 1600, 900});
    if (!loadingDone) {
      f.drawText({800, 450}, "loading resources...",
                 sf::Color::White, loadingText);
    } else {
      f.withAlpha(
          linearTween(0.3, 1, sineAnim(float(appState.uptime), 0.2)),
          [&]() {
            f.drawText({800, 450}, "press any key to begin",
                       sf::Color::White, loadingText);
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

bool ExecWrapper::handle(const sf::Event &event) {
  if (loadingDone && !mainCtrl) {
    if (event.type == sf::Event::KeyReleased
        or event.type == sf::Event::MouseButtonReleased) {
      mainCtrl = std::move(mainCtrlCtor(appState));
      animBegin = appState.uptime;
      areChildren({mainCtrl.get()});
      return true;
    }
  }

  return ui::Control::handle(event);
}

}

}
