#include "multiplayerui.h"

/**
 * MultiplayerLobby.
 */

MultiplayerLobby::MultiplayerLobby(MultiplayerConnection &shared) :
    MultiplayerView(shared) { }

void MultiplayerLobby::tick(float delta) {

}

void MultiplayerLobby::render(ui::Frame &f) {
  f.drawSprite(textureOf(Res::Lobby), {0, 0}, {0, 0, 1600, 900});
  int i = 0;
  sf::Color color;
  for (sky::Player &player : shared.arena->players) {
    f.drawText(
        {style.lobbyPlayersOffset,
         style.lobbyTopMargin + (i * style.lobbyFontSize)},
        {player.nickname}, style.lobbyFontSize,
        (player.team == 0) ? style.playerSpecColor : style.playerJoinedColor);
    i++;
  }
  // TODO: don't copy strings

  messageLog.render(f);
  chatEntry.render(f);
  readyButton.render(f);
}

bool MultiplayerLobby::handle(const sf::Event &event) {
  return false;
}

/**
 * MultiplayerGame.
 */

MultiplayerGame::MultiplayerGame(MultiplayerConnection &shared) :
    MultiplayerView(shared),
    renderSystem(shared.arena.sky.get_ptr()) { }

void MultiplayerGame::tick(float delta) {

}

void MultiplayerGame::render(ui::Frame &f) {

  // arena and arena->sky exist
  if (sky::Plane *plane = mShared.arena->sky->getPlane(myPlayer->pid)) {
    renderSystem->render(f, plane->state.pos);
  } else {
    renderSystem->render(f, {0, 0}); // TODO: panning in spectator mode
  }

  // score overlay
  const static sf::Vector2f scoreOverlayPos = 0.5f *
      sf::Vector2f(
          1600.0f - style.scoreOverlayDims.x,
          900.0f - style.scoreOverlayDims.y);
  f.drawSprite(
      textureOf(Res::ScoreOverlay), scoreOverlayPos,
      {0, 0, style.scoreOverlayDims.x, style.scoreOverlayDims.y});

  messageLog.render(f);
  chatEntry.render(f);
}

bool MultiplayerGame::handle(const sf::Event &event) {
  return false;
}

/**
 * MultiplayerScoring.
 */

MultiplayerScoring::MultiplayerScoring(MultiplayerConnection &shared) :
    MultiplayerView(shared) { }

void MultiplayerScoring::tick(float delta) {

}

void MultiplayerScoring::render(ui::Frame &f) {

}

bool MultiplayerScoring::handle(const sf::Event &event) {
  return false;
}
