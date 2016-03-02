#include "multiplayerui.h"

/**
 * MultiplayerLobby.
 */

MultiplayerLobby::MultiplayerLobby(MultiplayerShared &shared) :
    MultiplayerMode(shared) { }

void MultiplayerLobby::tick(float delta) {

}

void MultiplayerLobby::render(ui::Frame &f) {

}

bool MultiplayerLobby::handle(const sf::Event &event) {
  return false;
}

/**
 * MultiplayerGame.
 */

MultiplayerGame::MultiplayerGame(MultiplayerShared &shared) :
    MultiplayerMode(shared),
    renderSystem(shared.arena.sky.get_ptr()) { }

void MultiplayerGame::tick(float delta) {

}
void MultiplayerGame::render(ui::Frame &f) {

}
bool MultiplayerGame::handle(const sf::Event &event) {
  return false;
}

/**
 * MultiplayerScoring.
 */

MultiplayerScoring::MultiplayerScoring(MultiplayerShared &shared) :
    MultiplayerMode(shared) { }

void MultiplayerScoring::tick(float delta) {

}
void MultiplayerScoring::render(ui::Frame &f) {

}
bool MultiplayerScoring::handle(const sf::Event &event) {
  return false;
}
