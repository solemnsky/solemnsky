#include "multiplayerlobby.h"
#include "client/elements/style.h"

void MultiplayerLobby::doClientAction(const ClientAction action,
                                      const bool state) {
  switch (action) {
    case ClientAction::Spawn:
      break;
    case ClientAction::Chat: {
      if (state) chatInput.focus();
      break;
    }
    case ClientAction::Scoreboard:
      break;
  }
}

MultiplayerLobby::MultiplayerLobby(
    ClientShared &shared, MultiplayerShared &connection) :
    MultiplayerView(sky::ArenaMode::Lobby, shared, connection),

    specButton(appState, style.base.normalButton, style.multi.lobbyButtonPos,
               "SPECTATE"),
    redButton(appState, style.base.normalButton,
              style.multi.lobbyButtonPos + style.multi.lobbyButtonSep,
              "JOIN RED"),
    blueButton(appState, style.base.normalButton,
               style.multi.lobbyButtonPos + 2.0f * style.multi.lobbyButtonSep,
               "JOIN BLUE"),
    chatInput(appState, style.base.normalTextEntry,
              style.multi.chatPos, "[ENTER TO CHAT]") {
  areChildren({&specButton, &redButton, &blueButton, &chatInput});
}

void MultiplayerLobby::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerLobby::render(ui::Frame &f) {
  f.drawSprite(textureOf(ResID::Lobby), {0, 0}, {0, 0, 1600, 900});

  mShared.drawEventLog(f, style.multi.chatCutoff);
  f.drawText(
      style.multi.playerListPos, [&](Printer &p) {
        mShared.arena->forPlayers([&](const sky::Player &player) {
          p.setColor(0, 0, 0);
          if (player.team == 1) p.setColor(255, 0, 0);
          if (player.team == 2) p.setColor(0, 0, 255);
          p.print(player.nickname);
          p.breakLine();
        });
      }, style.multi.playerListText);

  ui::Control::render(f);
}

bool MultiplayerLobby::handle(const sf::Event &event) {
  if (ui::Control::handle(event)) return true;

  if (const auto action = shared.triggerClientAction(event)) {
    doClientAction(action->first, action->second);
    return true;
  }

  return false;
}

void MultiplayerLobby::reset() {
  ui::Control::reset();
}

void MultiplayerLobby::signalRead() {
  ui::Control::signalRead();

  if (specButton.clickSignal)
    mShared.requestTeamChange(0);
  if (redButton.clickSignal)
    mShared.requestTeamChange(1);
  if (blueButton.clickSignal)
    mShared.requestTeamChange(2);
  if (chatInput.inputSignal) {
    mShared.handleChatInput(chatInput.inputSignal.get());
  }
}

void MultiplayerLobby::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerLobby::onChangeSettings(const SettingsDelta &settings) {

}

