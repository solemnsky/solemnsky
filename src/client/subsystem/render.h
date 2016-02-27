/**
 * The render subsystem, managing plane animation state and drawing methods.
 */
#ifndef SOLEMNSKY_GRAPHICS_H
#define SOLEMNSKY_GRAPHICS_H

#include "client/util/resources.h"
#include "client/ui/control.h"
#include "client/ui/sheet.h"
#include "sky/sky.h"
#include <SFML/Graphics.hpp>

namespace sky {

const struct RndrParam {
  // might as well avoid magic values.. not sure if we'll want to refactor
  // this in the future
  RndrParam() = default;

  const float
  // how many degrees it rolls to either side
      rollAmount = 25,
  // how quickly it rolls 2 * rollAmount
      rollSpeed = 2,
  // how quickly it flips orientation
      flipSpeed = 2;

  const float spriteSize = 200;

  const sf::FloatRect afterburnArea = {-70, -10, 70, 20};

  const sf::FloatRect barArea = {-100, -100, 200, 30};
  const sf::Color
      throttleStall = sf::Color(0, 0, 0, 100),
      throttle = sf::Color::Black,
      health = sf::Color::Green,
      energy = sf::Color::Blue;
} rndrParam{};

/**
 * Represents a plane graphic on the display associated with our sky.
 * Each plane is associated with a PlaneGraphics; however, since when a plane
 * is removed, its graphics could stick around for a while in a death
 * animation, a PlaneGraphics can exist detached from a Plane.
 *
 * The lifecycle of a PlaneGraphics is consistent: it is constructed with a
 * Plane as a parent, which it attaches to; from that point on, it is tick()'d
 * along with the Render subsystem; at some point, it detaches from
 * the Plane when that plane is removed from the Sky, signaled by
 * PlaneGraphics::detach(); finally, it sets 'destroyed' to true and is
 * removed from the Render subsystem.
 */
struct PlaneGraphics {
  PlaneGraphics(const PID pid, const Plane &parent);
  PlaneGraphics &operator=(PlaneGraphics &&);
  PlaneGraphics(const PlaneGraphics &) = delete;
  PlaneGraphics &operator=(const PlaneGraphics &) = delete;


  const Plane *parent;
  optional<PID> pid;

  Angle roll;
  bool orientation;
  float flipState; // these two values contribute to the roll
  float rollState;
  bool destroyed; // true when the graphics has reached the singularity of
  // non-existence and can be removed without anybody noticing

  void tick(const float delta);
  // signal that a plane with a certain PID was removed
  void removePlane(const PID removedPid);
};

class Render: public Subsystem {
 private:
  std::list<PlaneGraphics> graphics;
  const ui::SpriteSheet sheet;

  /**
   * Render submethods.
   */
  float findView(const float viewWidth,
                 const float totalWidth,
                 const float viewTarget) const;
  void renderBars(ui::Frame &f,
                  std::vector<std::pair<float, const sf::Color &>> bars,
                  sf::FloatRect area);
  void renderPlaneGraphics(ui::Frame &f, const PlaneGraphics &graphics);

  /**
   * Subsystem listeners.
   */
  void tick(float delta) override;
  virtual void addPlane(const PID pid, Plane &plane) override;
  virtual void removePlane(const PID pid) override;

 public:
  Render(Sky *sky);
  ~Render();

  /**
   * Renders the game from a view centered on pos.
   */
  void render(ui::Frame &f, const sf::Vector2f &pos);
};

}

#endif //SOLEMNSKY_GRAPHICS_H
