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

class MultiplayerClient: public Game {
 private:
  ui::Button quitButton;
  ui::TextEntry chatEntry;
  ui::TextLog messageLog;


  /**
   * Connection details.
   */
  const sf::IpAddress serverAddress;
  const unsigned short serverPort, clientPort;

  /**
   * Connection state.
   */
  tg::Telegraph<sky::prot::ClientPacket, sky::prot::ServerPacket> telegraph;
  Cooldown pingCooldown;

  bool triedConnection; // have we sent a verbs requesting connection yet?
  bool connected;

  /**
   * Local model of the Arena / game state.
   */
  sky::PID myPID;
  sky::Arena arena;
  sky::Sky sky;
  sky::Render renderSystem;

  /**
   * Helpers.
   */
  void transmitServer(const sky::prot::ClientPacket &packet);
  void handleGamePacket(tg::Reception<sky::prot::ServerPacket> &&reception);

 public:
  MultiplayerClient(ClientShared &state,
                    const sf::IpAddress &serverAddress,
                    const unsigned short serverPort,
                    const unsigned short clientPort);

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
