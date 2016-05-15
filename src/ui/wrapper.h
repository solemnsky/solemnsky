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
/**
 * Wrapper control that makes the base Control's life easier.
 */
#pragma once
#include <memory>
#include "widgets/textlog.h"
#include "control.h"

namespace ui {

namespace detail {
/**
 * Supplied with a constructor for a main Control, this loads resources while
 * displaying a splash screen and fades into the main Control when resources
 * are ready. It also relays the wrapped control's `quitting` flag.
 */
// TODO: Make this threaded, currently the game loop locks up while resources
// are being loaded! You fool!
class ExecWrapper: public Control {
 private:
  double animBegin;
  bool drewScreen, loadingDone;
  TextFormat loadingText;
  std::function<std::unique_ptr<Control>(AppState &)> mainCtrlCtor;
  std::unique_ptr<Control> mainCtrl;

 public:
  ExecWrapper(AppState &appState,
              std::function<std::unique_ptr<Control>(AppState &)>
              mainCtrlCtor);

  virtual void poll(float delta) override final;
  virtual void tick(float delta) override final;
  virtual void render(Frame &f) override final;
  virtual bool handle(const sf::Event &event) override final;
};
}

}
