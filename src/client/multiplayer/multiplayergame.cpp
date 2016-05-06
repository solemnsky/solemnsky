#include "multiplayergame.h"
#include "client/elements/style.h"

void MultiplayerGame::doClientAction(const ClientAction action,
                                     const bool state) {
  switch (action) {
    case ClientAction::Spawn: {
      if (state) mShared.transmit(sky::ClientPacket::ReqSpawn());
      break;
    }
    case ClientAction::Chat: {
      if (state) chatInput.focus();
      break;
    }
    case ClientAction::Scoreboard: {
      scoreboardFocused = state;
      break;
    }
  }
}

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerShared &connection) :
    MultiplayerView(sky::ArenaMode::Game, shared, connection),
    chatInput(appState,
              style.base.normalTextEntry,
              style.multi.chatPos,
              "[ENTER TO CHAT]"),
    scoreboardFocused(false),
    skyRender(conn.arena, conn.skyManager, *conn.skyManager.getSky()),
    participation(*conn.getParticipation()) {
  areChildren({&chatInput});
}

void MultiplayerGame::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerGame::render(ui::Frame &f) {
  skyRender.render(
      f, participation.getPlane() ?
         participation.getPlane()->getState().physical.pos :
         sf::Vector2f(0, 0));

  if (chatInput.isFocused) mShared.drawEventLog(f, style.multi.chatCutoff);
  else mShared.drawEventLog(f, style.multi.chatIngameCutoff);

  ui::Control::render(f);

  if (scoreboardFocused) {
    // TODO: scoreboard
    f.drawText({20, 20}, "<scoreboard goes here>",
               sf::Color::Black, style.base.normalText);
  }
}

bool MultiplayerGame::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  if (conn.skyManager.isActive()) {
    if (auto action = shared.triggerSkyAction(event)) {
      mShared.transmit(
          sky::ClientPacket::ReqAction(action->first, action->second));
      return true;
    }
  }

  if (auto clientAction = shared.triggerClientAction(event)) {
    doClientAction(clientAction->first, clientAction->second);
    return true;
  }

  return false;
}

void MultiplayerGame::signalRead() {
  ui::Control::signalRead();
  if (chatInput.inputSignal) {
    mShared.handleChatInput(chatInput.inputSignal.get());
  }
}

void MultiplayerGame::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerGame::onChangeSettings(const SettingsDelta &settings) {

}

