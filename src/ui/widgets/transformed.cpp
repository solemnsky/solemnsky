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

TransformedBase::TransformedBase(Control &ctrl, const sf::Transform &transform) :
    Control(ctrl.references), ctrl(ctrl), transform(transform) {}

void TransformedBase::poll() {
  ctrl.poll();
}

void TransformedBase::tick(const TimeDiff delta) {
  ctrl.tick(delta);
}

void TransformedBase::render(Frame &f) {
  f.withTransform(transform, [&]() {
    ctrl.render(f);
  });
}

bool TransformedBase::handle(const sf::Event &event) {
  return ctrl.handle(transformEvent(inverseTransform, event));
}

void TransformedBase::reset() {
  ctrl.reset();
}

void TransformedBase::signalRead() {
  ctrl.signalRead();
}

void TransformedBase::signalClear() {
  ctrl.signalClear();
}

const sf::Transform TransformedBase::getTransform() const {
  return transform;
}

void TransformedBase::setTransform(const sf::Transform &newTransform) {
  transform = newTransform;
  inverseTransform = transform.getInverse();
}

}
