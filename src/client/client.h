#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include <ui/sheet.h>
#include "ui/ui.h"
#include "sky/sky.h"
#include "sky/client/render.h"


class ControlState {
private:
  optional<char> charFromEvent(const sf::Event &event);
  void handleKey(bool &state, const sf::Event &event);
public:
  bool leftCtrl{false}, rightCtrl{false},
      upCtrl{false}, downCtrl{false};

  void handle(const sf::Event &event);
  void setState(sky::PlaneState &state);
};

/**
 * The game client. This is going to get pretty huge fast, our luck is that a
 * lot of the UI can be neatly modularized.
 */
class Client : public ui::Control {
private:
  ControlState ctrlState;

  sky::Sky sky;
  sky::Render renderSystem;
  sky::Plane *plane;

public:
  Client();

  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;
};

#endif //SOLEMNSKY_CLIENT_H
