#include "multiplayerlobby.h"
#include "client/elements/style.h"

void MultiplayerLobby::doClientAction(const ClientAction action,
                                      const bool state) {
  if (action == ClientAction::Chat && state) chatInput.focus();
}

MultiplayerLobby::MultiplayerLobby(
    ClientShared &shared, MultiplayerShared &connection) :
    MultiplayerView(sky::ArenaMode::Lobby, shared, connection),

    specButton(style.base.normalButton, style.multi.lobbyButtonPos, "spectate"),
    redButton(style.base.normalButton,
              style.multi.lobbyButtonPos + style.multi.lobbyButtonSep,
              "join red"),
    blueButton(style.base.normalButton,
               style.multi.lobbyButtonPos + 2.0f * style.multi.lobbyButtonSep,
               "join blue"),
    chatInput(style.base.normalTextEntry,
              style.multi.chatPos, "[enter to chat]") {
  areChildren({&specButton, &redButton, &blueButton, &chatInput});
}

void MultiplayerLobby::tick(float delta) {
  ui::Control::tick(delta);
}

void MultiplayerLobby::render(ui::Frame &f) {
  f.drawSprite(textureOf(ResID::Lobby), {0, 0}, {0, 0, 1600, 900});

  mShared.drawEventLog(f, 300);
  f.drawText(
      style.multi.playerListPos, [&](ui::TextFrame &tf) {
        mShared.arena->forPlayers([&](const sky::Player &player) {
          tf.setColor(sf::Color::Black);
          if (player.team == 1) tf.setColor(sf::Color::Red);
          if (player.team == 2) tf.setColor(sf::Color::Blue);
          tf.print(player.nickname);
          tf.breakLine();
        });
      }, sf::Color::White, style.multi.playerListText);

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

