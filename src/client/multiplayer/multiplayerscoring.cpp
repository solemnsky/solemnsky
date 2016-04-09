#include "multiplayerscoring.h"
#include "client/elements/style.h"

MultiplayerScoring::MultiplayerScoring(
    ClientShared &shared, MultiplayerShared &connection) :
    MultiplayerView(sky::ArenaMode::Scoring, shared, connection) {
}

void MultiplayerScoring::tick(float delta) {

}

void MultiplayerScoring::render(ui::Frame &f) {

}

bool MultiplayerScoring::handle(const sf::Event &event) {
  return false;
}

void MultiplayerScoring::signalRead() {
  ui::Control::signalRead();
}

void MultiplayerScoring::signalClear() {
  ui::Control::signalClear();
}

void MultiplayerScoring::onChangeSettings(const SettingsDelta &settings) {

}
