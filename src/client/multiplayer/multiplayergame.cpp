#include "multiplayergame.h"
#include "client/elements/style.h"

void MultiplayerGame::doClientAction(const ClientAction action,
                                     const bool state) {
  switch (action) {
    case ClientAction::Spawn: {
      if (state && !mShared.plane->isSpawned())
        mShared.player->spawn({}, {200, 200}, 0);
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
    chatInput(style.base.normalTextEntry,
              style.multi.chatPos,
              "[enter to chat]"),
    scoreboardFocused(false) {
  areChildren({&chatInput});
}

void MultiplayerGame::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerGame::render(ui::Frame &f) {
  if (mShared.plane->vital) {
    mShared.skyRender->render(f, mShared.plane->vital->state.physical.pos);
  } else mShared.skyRender->render(f, {});
  ui::Control::render(f);
  if (chatInput.isFocused) mShared.drawEventLog(f, 900);
  else mShared.drawEventLog(f, 150);
  if (scoreboardFocused) {
    // TODO: scoreboard
    f.drawText({20, 20}, "<scoreboard goes here>",
               sf::Color::Black, style.base.normalText);
  }
}

bool MultiplayerGame::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  if (auto clientAction = shared.triggerClientAction(event)) {
    doClientAction(clientAction->first, clientAction->second);
    return true;
  }

  if (auto action = shared.triggerSkyAction(event)) {
    mShared.player->doAction(action->first, action->second);
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

