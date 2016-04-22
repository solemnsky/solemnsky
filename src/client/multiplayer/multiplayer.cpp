#include "multiplayer.h"
#include "client/elements/style.h"
#include "multiplayerlobby.h"
#include "multiplayergame.h"
#include "multiplayerscoring.h"

/**
 * Multiplayer.
 */

std::unique_ptr<MultiplayerView> Multiplayer::mkView() {
  switch (mShared.arena->getMode()) {
    case sky::ArenaMode::Lobby:
      return std::make_unique<MultiplayerLobby>(shared, mShared);
    case sky::ArenaMode::Game:
      return std::make_unique<MultiplayerGame>(shared, mShared);
    case sky::ArenaMode::Scoring:
      return std::make_unique<MultiplayerScoring>(shared, mShared);
  }
}

Multiplayer::Multiplayer(ClientShared &shared,
                         const std::string &serverHostname,
                         const unsigned short serverPort) :
    Game(shared, "multiplayer"),
    view(nullptr),
    mShared(shared, serverHostname, serverPort) { }

/**
 * Game interface.
 */

void Multiplayer::onChangeSettings(const SettingsDelta &settings) {
  mShared.onChangeSettings(settings);
  if (view) view->onChangeSettings(settings);

  if (mShared.player) {
    if (settings.nickname) {
      sky::PlayerDelta delta = mShared.player->zeroDelta();
      delta.nickname = *settings.nickname;
      mShared.transmit(sky::ClientPacket::ReqPlayerDelta(delta));
      // request a nickname change
    }
  }
}

void Multiplayer::onBlur() {

}

void Multiplayer::onFocus() {

}

void Multiplayer::doExit() {
  mShared.disconnect();
}

void Multiplayer::tick(float delta) {
  mShared.poll(delta);
  if (mShared.disconnected) quitting = true;
  if (mShared.disconnecting) return;

  if (mShared.arena) {
    mShared.arena->tick(delta);
    if (!view or view->target != mShared.arena->getMode()) view = mkView();
    view->tick(delta);
  }
}

void Multiplayer::render(ui::Frame &f) {
  if (!mShared.server) {
    if (mShared.disconnecting) {
      f.drawText({400, 400}, "Disconnecting...",
                 sf::Color::White, style.base.normalText);
      return;
    } else {
      f.drawText({400, 400}, {"Connecting..."}, sf::Color::White,
                 style.base.normalText);
      return;
    }
  }

  if (view) view->render(f);
}

bool Multiplayer::handle(const sf::Event &event) {
  if (view) return view->handle(event);
  return false;
}

void Multiplayer::reset() {
  if (view) view->reset();
}

void Multiplayer::signalRead() {
  if (view) view->signalRead();
}

void Multiplayer::signalClear() {
  if (view) view->signalClear();
}


