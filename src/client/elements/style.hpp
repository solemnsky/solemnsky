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
/**
 * Style settings.
 */
#pragma once
#include "ui/control.hpp"
#include "ui/widgets.hpp"

struct Style {
  /**
   * Basic style consistencies.
   */
  struct Base {
    // Font consistency.
    int smallFontSize, normalFontSize, titleFontSize;
    ui::TextFormat centeredText, normalText, debugText, debugRightText;

    // Color consistency.
    sf::Color buttonColor,
        buttonHotColor,
        buttonPressedColor,
        buttonInactiveColor,
        textColor,
        textAreaForeground,
        textAreaBackground,
        pageBgColor;

    float debugOpacity;

    // Animation consistency.
    float heatRate; // inverse of time it takes for a UI element to 'heat up'

    // Vanilla widgets.
    ui::Button::Style normalButton;
    ui::TextEntry::Style normalTextEntry;
    ui::Checkbox::Style normalCheckbox;

    Base();
  } base;

  /**
   * Splash screen.
   */
  struct Splash {
    float barWidth, barHeight, barPaddingTop;
    sf::Color barColor;
    ui::TextFormat titleFormat;

    Splash(const Base &base);
  } splash;

  /**
   * Top-level client interface.
   */
  struct Menu {
    sf::Vector2f pageSize;
    float unfocusedPageScale, pageMargins;

    // Position offsets. (See schema in media/source2d/board.png.)
    sf::Vector2f
        homeOffset,
        settingsOffset,
        listingOffset,
        quitButtonOffset,
        aboutButtonOffset,
        closeButtonOffset,
        backButtonOffset;
    sf::FloatRect
        homeArea,
        settingsArea,
        listingArea;

    // Some menu colors.
    sf::Color pageUnderlayColor, statusFontColor;

    // Page descriptions.
    int descSize;
    float pageDescMargin;

    float pageFocusAnimSpeed, // s^-1
        gameFocusAnimSpeed;

    ui::Button::Style highButtonStyle;
    ui::Button::Style lowButtonStyle;

    std::string backButtonText,
        closeButtonText,
        quitButtonText,
        aboutButtonText;

    ui::TextFormat menuDescText, gameDescText;

    float menuInGameFade; // the opacity of the UI when we open it
    // by escaping from the game (this opacity factor dissipates when a page
    // is focused)

    Menu(const Base &base);
  } menu;

  /**
   * The welcoming home page.
   */
  struct Home {
    sf::Vector2f tutorialButtonPos,
        localhostButtonPos,
        remoteButtonPos,
        serverEntryPos;

    Home(const Base &base);
  } home;

  /**
   * The function-packed settings page.
   */
  struct Settings {
    ui::TextEntry::Style textEntry;
    ui::Checkbox::Style checkbox;
    sf::Vector2f entryOffset;
    ui::TextFormat descText;

    sf::Vector2f column1Pos, column2Pos;
    float rowOffset;

    float pageButtonHeight;
    float generalButtonOffset,
        playerButtonOffset,
        controlsButtonOffset;
    sf::Color selectedTabButtonColor, unselectedTabButtonColor;
    // color of the selected tab button

    Settings(const Base &base);
  } settings;

  /**
   * The rather important server listing page.
   */
  struct Listing {
    // stub
    Listing();
  } listing;

  /**
   * The various game interfaces.
   */
  struct Game {
    sf::Vector2i scoreOverlayDims;
    float scoreOverlayTopMargin;
    int lobbyFontSize;

    float lobbyPlayersOffset,
        lobbyTopMargin,
        lobbyChatWidth,
        gameChatWidth;

    sf::Vector2f chatPos,
        messageLogPos,
        playerListPos,
        lobbyButtonPos, lobbyButtonSep,
        scoreboardOffset;
    sf::IntRect scoreboardDisplay;
    float scoreboardPaddingTop;
    float chatCutoff, chatIngameCutoff;

    sf::Color playerSpecColor, playerJoinedColor;
    std::string readyButtonActiveDesc, readyButtonDeactiveDesc;

    ui::TextFormat messageLogText, playerListText;

    Game(const Base &base);
  } game;

  /**
   * Render settings for the game.
   */
  struct SkyRender {
    float
        rollAmount, // how many degrees it rolls to either side
        rollSpeed, // how quickly it rolls 2 * rollAmount
        flipSpeed; // how quickly it flips orientation

    sf::FloatRect afterburnArea;

    sf::FloatRect barArea;
    sf::Color
        throttleStall,
        throttle,
        health,
        energy;
    float deathRate = 0.5;

    SkyRender();
  } skyRender;

  Style();
};

static const Style style;

