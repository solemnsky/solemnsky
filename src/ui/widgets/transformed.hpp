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
 * Wrapper to transform a Control, affecting display and event handling.
 */

#include "ui/control.hpp"

namespace ui {

namespace detail {

/**
 * Template-erased base class used by Transformed.
 */
class TransformedBase : public Control {
 private:
  Control &ctrl;
  sf::Transform transform;

 public:
  TransformedBase(Control &ctrl, const sf::Transform &transform);

  // Control impl.
  virtual bool poll() override;
  virtual void tick(const TimeDiff delta) override;
  virtual void render(Frame &f) override;
  virtual bool handle(const sf::Event &event) override;
  virtual void reset() override;
  virtual void signalRead() override;
  virtual void signalClear() override;

};

}

template<typename Ctrl>
class Transformed : public Control {
 private:
  detail::TransformedBase base;

 public:
  Transformed(std::unique_ptr<Ctrl> &&ctrl, const sf::Transform &transform) :
      Control(ctrl->references), ctrl(ctrl), base(*ctrl) {
    areChildren({&base});
  }

  std::unique_ptr<Ctrl> ctrl;
};

}

