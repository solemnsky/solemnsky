/**
 * Rendering the sky.
 */
#pragma once
#include "util/client/resources.h"
#include <list>
#include "ui/control.h"
#include "ui/sheet.h"
#include "sky/sky.h"
#include <SFML/Graphics.hpp>

namespace sky {

/**
 * Graphics state associated with a Participation.
 */
struct PlaneGraphics {
  PlaneGraphics(const Participation &plane);
  const Participation &plane;

  bool orientation;
  float flipState, rollState; // these two values contribute to the roll
  Angle roll() const;

  void tick(const float delta);
  void kill();
  void spawn();
};

/**
 * Subsystem, attaching additional graphics-related state to Players and
 * exposing top-level methods for rendering the game.
 */
class SkyRender: public Subsystem {
 private:
  const Sky &sky;

  std::map<PID, PlaneGraphics> graphics;
  const ui::SpriteSheet planeSheet;

  /**
   * Render submethods.
   */
  float findView(const float viewWidth,
                 const float totalWidth,
                 const float viewTarget) const;
  void renderBars(ui::Frame &f,
                  std::vector<std::pair<float, const sf::Color &>> bars,
                  sf::FloatRect area);
  void renderProps(ui::Frame &f, const Participation &planeVital);
  void renderPlaneGraphics(ui::Frame &f, const PlaneGraphics &graphics);
  void renderMap(ui::Frame &f);

  /**
   * Subsystem listeners.
   */
 protected:
  void registerPlayer(Player &player);
  void unregisterPlayer(Player &player);

  void onTick(const float delta) override;

 public:
  SkyRender(Arena &arena,
            const Sky &sky);
  ~SkyRender();

  /**
   * Renders the game from a view centered on pos.
   */
  void render(ui::Frame &f, const sf::Vector2f &pos);
  bool enableDebug;
};

}
