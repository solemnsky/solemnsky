/**
 * A whole bunch of values go here. Optimally, it should be fairly fast
 * recompile with changes here (just compile this file and link).
 */
#include "style.h"

Style::Base::Base() :
    buttonColor(132, 173, 181), // blue
    buttonHotColor(92, 189, 206), // hot blue
    buttonPressedColor(63, 208, 234), // less hot blue
    textColor(255, 255, 255), // not blue

    smallFontSize(20),
    normalFontSize(40),
    largeFontSize(60),

    normalButton(buttonColor, buttonHotColor, buttonPressedColor, textColor,
                 {200, 50}, 10, normalFontSize),
    greyedButton(buttonColor, buttonHotColor, buttonPressedColor, textColor,
                 {200, 50}, 10, normalFontSize) { }

Style::Menu::Menu(const Base &base) :
    unfocusedPageScale(500.0f / 1600.0f),

    homeOffset(182.812, 121.875),
    settingsOffset(917.187, 121.875),
    listingOffset(550.000, 495.250),
    quitButtonOffset(182.812, 610.875),
    aboutButtonOffset(1217.188, 610.875),
    closeButtonOffset(1300, 0),
    backButtonOffset(1067.18, 850),
    pageUnderlayColor(0, 0, 0, 20),
    statusFontColor(200, 200, 200),

    descSize(50),

    pageDescMargin(10),

    highButtonStyle(base.normalButton),
    lowButtonStyle(base.normalButton),

    backButtonText("main menu"),
    closeButtonText("close game"),
    quitButtonText("quit"),
    aboutButtonText("about"),
    menuInGameFade(0.7) {
  highButtonStyle.fontSize = 50;
  highButtonStyle.dimensions.y = 70;

  lowButtonStyle.fontSize = 40;
  lowButtonStyle.dimensions.x = 300;
}

Style::Home::Home(const Base &base) :
    tutorialButtonPos(500, 300),
    localhostButtonPos(1100, 300),
    tutorialButtonDesc("start tutorial"),
    localhostButtonDesc("connect to localhost") {

}

Style::Style() :
    base(),
    menu(base),
    home(base) { }

