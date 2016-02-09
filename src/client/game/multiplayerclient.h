/**
 * Multiplayer client.
 */
#ifndef SOLEMNSKY_MULTICLIENT_H
#define SOLEMNSKY_MULTICLIENT_H

#include "client/elements/elements.h"
#include "telegraph/telegraph.h"
#include "sky/protocol.h"
#include "sky/sky.h"
#include "sky/arena.h"
#include "client/subsystem/render.h"

class MultiplayerClient : public Game {
private:
  ui::Button quitButton;
  ui::TextEntry chatEntry;
  ui::TextLog messageLog;

  tg::Telegraph<sky::prot::ClientPacket, sky::prot::ServerPacket> telegraph;
  Cooldown pingCooldown;

  /**
   * Target server.
   */
  const sf::IpAddress targetServer;
  const unsigned short targetPort;
  
  /**
   * Helpers.
   */
  void handleGamePacket(tg::Reception<sky::prot::ServerPacket> &&reception);

  /**
   * Multiplayer protocol state.
   */
  bool triedConnection; // have we sent a verbs requesting connection yet?
  bool connected;

  sky::PID myPID;
  sky::Arena arena;
  sky::Sky sky;
  sky::Render renderSystem;

public:
  MultiplayerClient(ClientShared &state,
                    const sf::IpAddress &targetServer,
                    const unsigned short targetPort);

  /**
   * Game interface.
   */
  void onLooseFocus() override;
  void onFocus() override;

  /**
   * Control interface.
   */
  void tick(float delta) override;
  void render(ui::Frame &f) override;
  bool handle(const sf::Event &event) override;

  void signalRead() override;
  void signalClear() override;
};

#endif //SOLEMNSKY_MULTICLIENT_H
