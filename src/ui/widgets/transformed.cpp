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
#include "transformed.hpp"
#include "util/clientutil.hpp"

namespace ui {

detail::TransformedBase::TransformedBase(Control &ctrl, const sf::Transform &transform) :
    Control(ctrl.references), ctrl(ctrl), transform(transform) {}

bool detail::TransformedBase::poll() {
  return ctrl.poll();
}

void detail::TransformedBase::tick(const TimeDiff delta) {
  ctrl.tick(delta);
}

void detail::TransformedBase::render(Frame &f) {
  f.withTransform(transform, [&]() {
    ctrl.render(f);
  });
}

bool detail::TransformedBase::handle(const sf::Event &event) {
  return ctrl.handle(transformEvent(transform, event));
}

void detail::TransformedBase::reset() {
  Control::reset();
}

void detail::TransformedBase::signalRead() {
  Control::signalRead();
}

void detail::TransformedBase::signalClear() {
  Control::signalClear();
}

}
