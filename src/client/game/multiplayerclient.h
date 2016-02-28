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
  struct Style {
    sf::Vector2i scoreOverlayDims{1330, 630};
    float scoreOverlayTopMargin = 100;

    float lobbyPlayersOffset = 1305,
        lobbyTopMargin = 205,
        lobbyChatWidth = 1220,
        lobbyFontSize = 40;
  } style;

  /**
   * Connection state / network stuff.
   */
  tg::Host host;
  ENetPeer *server;
  tg::Telegraph<sky::ServerPacket, sky::ClientPacket> telegraph;
  Cooldown pingCooldown;

  bool triedConnection; // have we sent a verbs requesting connection yet?

  bool disconnecting;
  Cooldown disconnectTimeout;

  ENetEvent event; // volatile

  /**
   * Local model of the Arena / game state.
   */
  sky::Player *myPlayer;
  optional<sky::Arena> arena;
  optional<sky::RenderSystem> renderSystem;

  /**
   * Graphics and UI.
   */
  ui::TextEntry chatEntry;
  ui::TextLog messageLog;
  bool scoreOverlayFocused;

  void setUI(const sky::ArenaMode mode); // set some style state on widgets
  void renderLobby(ui::Frame &f);
  void renderGame(ui::Frame &f);
  void renderScoring(ui::Frame &f);
  void renderScoreOverlay(ui::Frame &f);

  /**
   * Networking submethods.
   */
  void transmitServer(const sky::ClientPacket &packet);
  // handle a network poll, assuming we're connected
  bool processPacket(const sky::ServerPacket &event);

 public:
  MultiplayerClient(ClientShared &state,
                    const std::string &serverHostname,
                    const unsigned short serverPort);

  /**
   * Game interface.
   */
  void onBlur() override;
  void onFocus() override;
  void onChangeSettings(const SettingsDelta &settings) override;
  void doExit() override;

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
