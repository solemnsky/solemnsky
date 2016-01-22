/**
 * The render subsystem, managing plane animation state and drawing methods.
 */
#ifndef SOLEMNSKY_GRAPHICS_H
#define SOLEMNSKY_GRAPHICS_H

#include "base/resources.h"
#include <SFML/Graphics.hpp>
#include "ui/control.h"
#include "ui/sheet.h"
#include "sky/subsystem.h"

namespace sky {
namespace detail {

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
} rndrParam;

/**
 * Plain data structure with all the animation state of a plane, alive or not.
 * This is derived from the Plane over time after having been thrust
 * into existence at the joining of a plane into the sky, reset to default
 * at each respawn. It is is not necessary to sync this over the network.
 */
struct PlaneAnimState {
  PlaneAnimState();

  Angle roll;
  bool orientation;

  float flipState; // these two values contribute to the roll
  float rollState;

  /**
   * Mutating state.
   */
  void tick(class PlaneHandle *parent, const float delta);
  void reset(); // when the plane respawns
  // death is animated in the a normal course of events, but respawning changes
  // everything anew
};
}

class Render : public Subsystem {
private:
  std::map<int, detail::PlaneAnimState> animState;
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
  void renderPlane(ui::Frame &f, const int pid, PlaneHandle &plane);

  /**
   * Subsystem listeners.
   */
  void tick(float delta) override;
  void joinPlane(const PID pid, PlaneHandle *plane) override;
  void quitPlane(const PID pid) override;
  void spawnPlane(const PID pid, PlaneHandle *plane) override;
  void killPlane(const PID pid, PlaneHandle *plane) override;

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
