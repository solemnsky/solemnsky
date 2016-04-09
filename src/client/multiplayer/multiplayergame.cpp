#include "multiplayergame.h"
#include "client/elements/style.h"

MultiplayerGame::MultiplayerGame(
    ClientShared &shared, MultiplayerShared &connection) :
    MultiplayerView(sky::ArenaMode::Game, shared, connection),
    chatInput(style.base.normalTextEntry,
              style.multi.chatPos,
              "[enter to chat]") {
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
}

bool MultiplayerGame::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  if (event.type == sf::Event::KeyPressed) {
    if (event.key.code == sf::Keyboard::Return) {
      chatInput.focus();
      return true;
    }
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

