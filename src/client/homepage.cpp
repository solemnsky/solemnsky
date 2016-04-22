#include "homepage.h"
#include "util/methods.h"
#include "elements/style.h"
#include "client/multiplayer/multiplayer.h"
#include "client/tutorial/tutorial.h"

HomePage::HomePage(ClientShared &clientState) :
    Page(clientState),
    tutorialButton(appState, style.base.normalButton,
                   style.home.tutorialButtonPos,
                   style.home.tutorialButtonDesc),
    localhostButton(appState, style.base.normalButton,
                    style.home.localhostButtonPos,
                    style.home.localhostButtonDesc) {
  appLog("constructed home page");
  areChildren({&tutorialButton, &localhostButton});
}

void HomePage::tick(float delta) {
  ui::Control::tick(delta);
}

void HomePage::onChangeSettings(const SettingsDelta &settings) {

}

void HomePage::onBlur() {

}

void HomePage::render(ui::Frame &f) {
  appLog("rendering home page");
  drawBackground(f);
  ui::Control::render(f);
}

bool HomePage::handle(const sf::Event &event) {
  return ui::Control::handle(event);
}

void HomePage::reset() {
  ui::Control::reset();
}

void HomePage::signalRead() {
  ui::Control::signalRead();

  if (tutorialButton.clickSignal)
    shared.beginGame(std::make_unique<Tutorial>(shared));

  if (localhostButton.clickSignal)
    shared.beginGame(
        std::make_unique<Multiplayer>(shared, "localhost", 4242));
}

void HomePage::signalClear() {
  ui::Control::signalClear();
}



