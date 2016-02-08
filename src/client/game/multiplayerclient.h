/**
 * Multiplayer client.
 */
#ifndef SOLEMNSKY_MULTICLIENT_H
#define SOLEMNSKY_MULTICLIENT_H

#include "client/elements/elements.h"
#include "telegraph/telegraph.h"
#include "sky/protocol.h"

class MultiplayerClient : public Game {
private:
  ui::Button quitButton;
  ui::TextEntry chatEntry;

  tg::Telegraph<sky::prot::ClientPacket, sky::prot::ServerPacket> telegraph;
  Cooldown pingCooldown;

public:
  MultiplayerClient(ClientShared &state);

  /**
   * Control interface.
   */
  virtual void tick(float delta) override;
  virtual void render(ui::Frame &f) override;
  virtual bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_MULTICLIENT_H
