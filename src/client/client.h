#ifndef SOLEMNSKY_CLIENT_H
#define SOLEMNSKY_CLIENT_H

#include <ui/sheet.h>
#include "ui/ui.h"
#include "sky/sky.h"
#include "sky/client/render.h"
#include "gamecontroller.h"
#include "tabselector.h"

/**
 * The main client app. This is going to get pretty huge fast, our luck is that
 * the UI can be neatly modularized.
 */
class Client : public ui::Control {
private:
  TabSelector tabSelector;

public:
  Client();

  void tick(float delta) override;
  void render(ui::Frame &f) override;
  void handle(const sf::Event &event) override;

  virtual void signalRead() override;
  virtual void signalClear() override;
};

#endif //SOLEMNSKY_CLIENT_H
