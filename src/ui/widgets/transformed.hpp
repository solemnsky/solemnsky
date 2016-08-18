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
#pragma once
#include "ui/control.hpp"

namespace ui {

/**
 * Template-erased base class used by Transformed.
 */
class TransformedBase : public Control {
 private:
  Control &ctrl;

  // Transformation and cached inverse.
  sf::Transform transform;
  sf::Transform inverseTransform;

 public:
  TransformedBase(Control &ctrl, const sf::Transform &transform);

  // Control impl.
  virtual void poll() override;
  virtual void tick(const TimeDiff delta) override;
  virtual void render(Frame &f) override;
  virtual bool handle(const sf::Event &event) override;
  virtual void reset() override;
  virtual void signalRead() override;
  virtual void signalClear() override;

  // Transformation.
  const sf::Transform getTransform() const;
  void setTransform(const sf::Transform &newTransform);

};

/**
 * Given an underlying control and a transformation, make a new wrapper control that acts
 * like the underlying control, but is translated by the supplied isometry.
 */
template<typename Ctrl>
class Transformed : public Control {
 public:
  std::unique_ptr<Ctrl> ctrlPtr;

 public:
  Transformed(std::unique_ptr<Ctrl> &&ctrl, const sf::Transform &transform) :
      Control(ctrl->references),
      ctrlPtr(std::move(ctrl)),
      base(*ctrlPtr, transform),
      ctrl(*ctrlPtr) {
    areChildren({&base});
  }

  // Type-erased base object with everything but access to the original Ctrl exposed.
  TransformedBase base;

  // Underlying Control.
  Ctrl &ctrl;

  // Transformation -- relays to the base.
  inline const sf::Transform getTransform() const {
    return base.getTransform();
  }

  inline void setTransform(const sf::Transform &newTransform) {
    base.setTransform(newTransform);
  }
};

}

