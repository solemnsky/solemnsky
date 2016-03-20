/**
 * A whole bunch of values go here.
 * TODO: When I'm back to working on the UI, it might be worth organising this.
 */
#include "style.h"

Style::Base::Base() :
    pageSize(1600, 900),

    normalText(24, {},
               ui::HorizontalAlign::Left, ui::VerticalAlign::Top,
               fontOf(Res::Font)),

    buttonColor(132, 173, 181), // blue
    buttonHotColor(92, 189, 206), // hot blue
    buttonPressedColor(63, 208, 234), // less hot blue
    buttonInactiveColor(200, 200, 200),
    textColor(255, 255, 255), // not blue
    textAreaForeground(0, 0, 0),
    textAreaBackground(255, 255, 255),

    heatRate(10),

    smallFontSize(20),
    normalFontSize(40),
    largeFontSize(60),

    pageMargins(50),
    pageBgColor{136, 156, 255},

    normalButton(buttonColor, buttonHotColor, buttonPressedColor,
                 buttonInactiveColor, textColor,
                 {200, 50}, heatRate, normalFontSize),

    normalTextEntry(buttonColor, buttonHotColor, textAreaBackground,
                    textColor, textAreaForeground, {500, 40},
                    normalFontSize, heatRate),
    normalCheckbox(buttonColor, buttonHotColor, buttonPressedColor,
                   buttonInactiveColor, textColor,
                   {40, 40}, heatRate, normalFontSize) { }

Style::Splash::Splash() :
    loadingText(60, {},
                ui::HorizontalAlign::Center, ui::VerticalAlign::Middle,
                fontOf(Res::Font)) { }

Style::Menu::Menu(const Style::Base &base) :
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

    homeArea(homeOffset, base.pageSize * unfocusedPageScale),
    settingsArea(settingsOffset, base.pageSize * unfocusedPageScale),
    listingArea(listingOffset, base.pageSize * unfocusedPageScale),

    descSize(base.normalFontSize),

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

Style::Home::Home(const Style::Base &base) :
    tutorialButtonPos(500, 300),
    localhostButtonPos(1100, 300),
    tutorialButtonDesc("start tutorial"),
    localhostButtonDesc("connect to localhost") { }

Style::Settings::Settings(const Style::Base &base) :
    textEntry(base.normalTextEntry),
    checkbox(base.normalCheckbox),
    entryOffset(150, 4),

    debugChooserPos(100, 100),
    nicknameChooserPos(100, 100),
    pageButtonHeight(800),
    generalButtonOffset(700 / 6),
    playerButtonOffset(generalButtonOffset + 300 + 700 / 3),
    controlsButtonOffset(playerButtonOffset + 300 + 700 / 3),
    selectedTabButtonColor(100, 100, 100),
    unselectedTabButtonColor(base.buttonColor) { }

Style::Listing::Listing() { }

Style::Multi::Multi(const Style::Base &base) :
    scoreOverlayDims{1330, 630},
    scoreOverlayTopMargin(100),
    lobbyFontSize(base.normalFontSize),

    lobbyPlayersOffset(1250),
    lobbyTopMargin(205),
    lobbyChatWidth(900),
    gameChatWidth(500),

    chatPos(20, 850),
    messageLogPos(20, 840),
    playerListPos(1250, 150),
    lobbyButtonPos(lobbyChatWidth + 100, lobbyTopMargin),
    lobbyButtonSep(0, 100),

    playerSpecColor(255, 255, 255),
    playerJoinedColor(0, 255, 0),
    readyButtonActiveDesc("ready!"),
    readyButtonDeactiveDesc("cancel"),

    messageLogText(base.normalText),
    playerListText(base.normalText) { }

Style::Style() :
    base(),
    menu(base),
    home(base),
    settings(base),
    multi(base) { }


