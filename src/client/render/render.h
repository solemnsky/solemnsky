/**
 * Rendering the sky.
 */
#pragma once
#include "client/util/resources.h"
#include <list>
#include "client/ui/control.h"
#include "client/ui/sheet.h"
#include "sky/sky.h"
#include <SFML/Graphics.hpp>

namespace sky {

/**
 * Graphics state associated with a Plane.
 */
struct PlaneGraphics {
  PlaneGraphics(const Plane &plane);

  const Plane &plane;

  bool wasDead;
  Angle roll;
  bool orientation;
  float flipState, rollState; // these two values contribute to the roll
  Clamped life;

  void onRespawn();

  void tick(const float delta);
};

class SkyRender: public Subsystem {
 private:
  std::map<PID, PlaneGraphics> graphics;
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
 protected:
  void registerPlayer(Player &player);
  void unregisterPlayer(Player &player);

  void onTick(const float delta) override;

 public:
  SkyRender(const Arena *arena, const Sky *sky);
  ~SkyRender();

  /**
   * Renders the game from a view centered on pos.
   */
  void render(ui::Frame &f, const sf::Vector2f &pos);
};

}
