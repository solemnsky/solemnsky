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
    screenSize(1600, 900),

    smallFontSize(25),
    normalFontSize(30),
    titleFontSize(50),
    debugFontSize(18),

    centeredText(normalFontSize, {}, ui::HorizontalAlign::Center, ui::VerticalAlign::Top),
    normalText(normalFontSize, {}, ui::HorizontalAlign::Left, ui::VerticalAlign::Top),
    debugText(debugFontSize, {}, ui::HorizontalAlign::Left, ui::VerticalAlign::Top),
    debugRightText(debugFontSize, {}, ui::HorizontalAlign::Right, ui::VerticalAlign::Top),

    dark(1, 14, 32),
    backgroundDark(7, 34, 75),
    background(29, 63, 109),
    accentDark(56, 90, 137),
    accentLight(106, 135, 176),

    freeTextColor(255, 255, 255),
    textColor(255, 255, 255),
    textAreaForeground(0, 0, 0),
    textAreaBackground(255, 255, 255),

    debugOpacity(0.7),

    heatRate(10),

    normalButton(accentLight, accentDark, backgroundDark,
                 backgroundDark, textColor,
                 {200, 50}, heatRate, normalFontSize),
    normalTextEntry(accentLight, accentDark, textAreaBackground,
                    textColor, textAreaForeground, {500, 40},
                    normalFontSize, heatRate),
    normalCheckbox(normalButton),

    debugMargin(380),
    debugHeight(400),
    debugBackground(255, 255, 255, 140) {
  normalCheckbox.dimensions = {50, 50};
}

Style::Splash::Splash(const Base &base) :
    barWidth(800),
    barHeight(30),
    barPaddingTop(50),
    transition(1.5),
    barColor(sf::Color::White),
    titleFormat(base.titleFontSize, {}, ui::HorizontalAlign::Center,
                ui::VerticalAlign::Middle, sf::Color::Black, 2) { }

Style::Menu::Menu(const Style::Base &base) :
    unfocusedPageScale(500.0f / 1600.0f),
    pageMargins(50),
    pageSize(base.screenSize - 2.0f * sf::Vector2f(pageMargins, pageMargins)),
    pageYOffset(-20),

    homeOffset(182.812, 121.875),
    settingsOffset(917.187, 121.875),
    listingOffset(550.000, 495.250),
    quitButtonOffset(182.812, 610.875),
    aboutButtonOffset(1217.188, 610.875),
    closeButtonOffset(1600 - 200 - 200, 850 + pageYOffset / 2.0f),
    backButtonOffset(200, 850 + pageYOffset / 2.0f),

    homeArea(homeOffset, base.screenSize * unfocusedPageScale),
    settingsArea(settingsOffset, base.screenSize * unfocusedPageScale),
    listingArea(listingOffset, base.screenSize * unfocusedPageScale),

    pageBgColor(base.background),
    pageUnderlayColor(255, 255, 255, 15),
    statusFontColor(base.freeTextColor),
    gameOverlayColor(0, 0, 0, 150),

    descSize(base.normalFontSize),

    pageDescMargin(0),

    pageFocusAnimSpeed(4),
    gameFocusAnimSpeed(6),

    highButtonStyle(base.normalButton),
    lowButtonStyle(base.normalButton),

    backButtonText("main menu"),
    closeButtonText("close game"),
    quitButtonText("quit"),
    aboutButtonText("about"),

    menuDescText(base.normalText),
    gameDescText(base.normalText),

    menuInGameFade(1),
    menuNormalFade(0.9),

    quitDescriptionFade(5) {
  highButtonStyle.fontSize = 50;
  highButtonStyle.dimensions.y = 70;

  lowButtonStyle.fontSize = 40;
  lowButtonStyle.dimensions.x = 300;

  menuDescText.size = 50;
  menuDescText.vertical = ui::VerticalAlign::Bottom;

  gameDescText.horizontal = ui::HorizontalAlign::Right;
  gameDescText.outlineColor = sf::Color::Black;
  gameDescText.outlineThickness = 2;
  menuDescText.outlineColor = sf::Color::Black;
  menuDescText.outlineThickness = 2;
}

Style::Home::Home(const Style::Base &) :
    tutorialButtonPos(500, 300),
    localhostButtonPos(1100, 300),
    remoteButtonPos(800, 600),
    serverEntryPos(800, 500) { }

Style::Settings::Settings(const Style::Menu &menu, const Style::Base &base) :
    textEntry(base.normalTextEntry),
    checkbox(base.normalCheckbox),
    descText(40, {}, ui::HorizontalAlign::Left, ui::VerticalAlign::Middle),
    sectionButton(base.normalButton),

    // We allocate 150 px for the description sizes, and 100 px side margin.
    column1Pos(menu.pageMargins + 100 + 150, 80),
    column2Pos(menu.pageMargins + menu.pageSize.x - 100 - 500, 80),

    rowOffset(80),
    pageButtonHeight(800),
    generalButtonOffset(menu.pageMargins),
    playerButtonOffset(menu.pageMargins + menu.pageSize.x / 3),
    controlsButtonOffset(menu.pageMargins + 2 * menu.pageSize.x / 3) {
  sectionButton.dimensions.x = menu.pageSize.x / 3;
}

Style::Listing::Listing() { }

Style::Game::Game(const Style::Base &base) :
    scoreOverlayDims{1330, 630},
    scoreOverlayTopMargin(100),

    lobbyPlayersOffset(1250),
    lobbyTopMargin(205),
    lobbyChatWidth(900),
    gameChatWidth(500),

    messagePos(20, 850),
    playerListPos(1250, 200),
    lobbyButtonPos(lobbyChatWidth + 100, lobbyTopMargin),
    lobbyButtonSep(0, 100),
    scoreboardOffset(100, 100),
    scoreboardDisplay(0, 0, 1400, 700),

    scoreboardPaddingTop(100),
    chatYPadding(12),

    playerSpecColor(255, 255, 255),
    playerJoinedColor(0, 255, 0),

    messageEntry(base.normalTextEntry),
    messageLog(700, 800, 300, 10, 9, base.normalFontSize),

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
    deathRate(0.5),
    planeGraphicsScale(1.5) { }

Style::Style() :
    base(),
    splash(base),
    menu(base),
    home(base),
    settings(menu, base),
    game(base) { }


