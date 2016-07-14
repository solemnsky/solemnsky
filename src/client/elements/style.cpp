/**
 * solemnsky: the open-source multiplayer competitive 2D plane game
 * Copyright (C) 2016  Chris Gadzinski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "style.hpp"

// Lots of magic values in this file. Have fun!

Style::Base::Base() :
    pageSize(1600, 900),

    centeredText(30, {}, ui::HorizontalAlign::Center, ui::VerticalAlign::Top),
    normalText(30, {}, ui::HorizontalAlign::Left, ui::VerticalAlign::Top),
    debugText(25, {}, ui::HorizontalAlign::Left, ui::VerticalAlign::Top),
    debugRightText(25, {}, ui::HorizontalAlign::Right, ui::VerticalAlign::Top),

    buttonColor(132, 173, 181), // blue
    buttonHotColor(92, 189, 206), // hot blue
    buttonPressedColor(63, 208, 234), // less hot blue
    buttonInactiveColor(200, 200, 200),
    textColor(255, 255, 255), // not blue
    textAreaForeground(0, 0, 0),
    textAreaBackground(255, 255, 255),

    heatRate(10),

    smallFontSize(20),
    normalFontSize(30),
    largeFontSize(40),

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
                   {40, 40}, heatRate, normalFontSize),

    debugOpacity(0.7) {}

Style::Splash::Splash() :
    barWidth(800),
    barHeight(30),
    barPaddingTop(50),
    barColor(sf::Color::White),
    titleFormat(50, {}, ui::HorizontalAlign::Center,
                ui::VerticalAlign::Middle) {}

Style::Menu::Menu(const Style::Base &base) :
    unfocusedPageScale(500.0f / 1600.0f),

    homeOffset(182.812, 121.875),
    settingsOffset(917.187, 121.875),
    listingOffset(550.000, 495.250),
    quitButtonOffset(182.812, 610.875),
    aboutButtonOffset(1217.188, 610.875),
    closeButtonOffset(1300, 0),
    backButtonOffset(1067.18, 850),

    homeArea(homeOffset, base.pageSize * unfocusedPageScale),
    settingsArea(settingsOffset, base.pageSize * unfocusedPageScale),
    listingArea(listingOffset, base.pageSize * unfocusedPageScale),

    pageUnderlayColor(0, 0, 0, 20),
    statusFontColor(200, 200, 200),

    descSize(base.normalFontSize),

    pageDescMargin(10),

    pageFocusAnimSpeed(4),
    gameFocusAnimSpeed(6),

    highButtonStyle(base.normalButton),
    lowButtonStyle(base.normalButton),

    backButtonText("MAIN MENU"),
    closeButtonText("CLOSE GAME"),
    quitButtonText("QUIT"),
    aboutButtonText("ABOUT"),

    menuDescText(base.normalText),
    gameDescText(base.normalText),

    menuInGameFade(0.7) {
  highButtonStyle.fontSize = 50;
  highButtonStyle.dimensions.y = 70;

  lowButtonStyle.fontSize = 40;
  lowButtonStyle.dimensions.x = 300;

  menuDescText.size = 50;
  menuDescText.vertical = ui::VerticalAlign::Bottom;

  gameDescText.horizontal = ui::HorizontalAlign::Right;
}

Style::Home::Home(const Style::Base &) :
    tutorialButtonPos(500, 300),
    localhostButtonPos(1100, 300),
    remoteButtonPos(800, 600),
    serverEntryPos(800, 500) {}

Style::Settings::Settings(const Style::Base &base) :
    textEntry(base.normalTextEntry),
    checkbox(base.normalCheckbox),
    entryOffset(150, -0.5f * base.normalTextEntry.dimensions.y),
    descText(40, {}, ui::HorizontalAlign::Left, ui::VerticalAlign::Middle),

    column1Pos(300, 80),
    column2Pos(800, 80),
    rowOffset(80),
    pageButtonHeight(800),
    generalButtonOffset(700 / 6),
    playerButtonOffset(generalButtonOffset + 300 + 700 / 3),
    controlsButtonOffset(playerButtonOffset + 300 + 700 / 3),
    selectedTabButtonColor(100, 100, 100),
    unselectedTabButtonColor(base.buttonColor) {}

Style::Listing::Listing() {}

Style::Multi::Multi(const Style::Base &base) :
    scoreOverlayDims{1330, 630},
    scoreOverlayTopMargin(100),
    lobbyFontSize(base.normalFontSize),

    lobbyPlayersOffset(1250),
    lobbyTopMargin(205),
    lobbyChatWidth(900),
    gameChatWidth(500),

    chatPos(20, 850),
    messageLogPos(20, 850 - 15),
    playerListPos(1250, 200),
    lobbyButtonPos(lobbyChatWidth + 100, lobbyTopMargin),
    lobbyButtonSep(0, 100),
    scoreboardOffset(100, 100),
    scoreboardDisplay(0, 0, 1400, 700),
    scoreboardPaddingTop(100),

    chatCutoff(500),
    chatIngameCutoff(150),

    playerSpecColor(255, 255, 255),
    playerJoinedColor(0, 255, 0),
    readyButtonActiveDesc("READY!"),
    readyButtonDeactiveDesc("CANCEL"),

    messageLogText(base.normalText),
    playerListText(base.normalText) {
  messageLogText.vertical = ui::VerticalAlign::Bottom;
}

Style::SkyRender::SkyRender() :
    rollAmount(25),
    rollSpeed(2),
    flipSpeed(2),
    afterburnArea(-70, -10, 70, 20),
    barArea(-100, -100, 200, 30),
    throttleStall(0, 0, 0, 100),
    throttle(sf::Color::Black),
    health(sf::Color::Green),
    energy(sf::Color::Blue),
    deathRate(0.5) {}

Style::Style() :
    base(),
    menu(base),
    home(base),
    settings(base),
    multi(base) {}


