#include "homepage.h"
#include "util/methods.h"
#include "elements/style.h"
#include "client/multiplayer/multiplayer.h"
#include "client/tutorial/tutorial.h"

HomePage::HomePage(ClientShared &clientState) :
    Page(clientState),
    tutorialButton(style.base.normalButton,
                   style.home.tutorialButtonPos,
                   style.home.tutorialButtonDesc),
    localhostButton(style.base.normalButton,
                    style.home.localhostButtonPos,
                    style.home.localhostButtonDesc) { }

void HomePage::onBlur() {
  tutorialButton.reset();
  localhostButton.reset();
}

void HomePage::onFocus() { }

void HomePage::tick(float delta) {
  tutorialButton.tick(delta);
  localhostButton.tick(delta);
}

void HomePage::onChangeSettings(const SettingsDelta &settings) {

}

void HomePage::render(ui::Frame &f) {
  drawBackground(f);

  tutorialButton.render(f);
  localhostButton.render(f);
}

bool HomePage::handle(const sf::Event &event) {
  if (tutorialButton.handle(event)) return true;
  return localhostButton.handle(event);
}

void HomePage::signalRead() {
  if (tutorialButton.clickSignal)
    shared.beginGame(std::make_unique<Tutorial>(shared));

  if (localhostButton.clickSignal)
    shared.beginGame(
        std::make_unique<Multiplayer>(shared, "localhost", 4242));
}

void HomePage::signalClear() {
  tutorialButton.signalClear();
  localhostButton.signalClear();
}

