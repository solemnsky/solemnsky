/**
 * Multiplayer client.
 */
#ifndef SOLEMNSKY_MULTICLIENT_H
#define SOLEMNSKY_MULTICLIENT_H

#include "game.h"
#include "telegraph/telegraph.h"

class MultiplayerClient : public Game {
private:
  ui::Button quitButton;
  tg::Telegraph telegraph;
  tg::Packet buffer;

  Cooldown pingCooldown;

public:
  MultiplayerClient(ClientState *state);

  /**
   * Control interface.
   */
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual void handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_MULTICLIENT_H
