/**
 * Default multiplayer server.
 */
#pragma once
#include "server/server.h"

class VanillaServer: public Server {


 public:
 protected:
  // subsystem callbacks
  void registerPlayer(sky::Player &player) override;
  void unregisterPlayer(sky::Player &player) override;
  void onTick(const float delta) override;

  // server callbacks
  virtual void onPacket(ENetPeer *const client,
                        sky::Player &player,
                        const sky::ClientPacket &packet);

 public:

  VanillaServer(ServerShared &telegraphy, sky::Arena &arena, sky::Sky &sky);

};
