/**
 * Style settings.
 */
#pragma once
#include "ui/control.h"
#include "ui/widgets/widgets.h"

struct Style {
  /**
   * Basic style consistencies.
   */
  struct Base {
    sf::Vector2f pageSize;

    ui::TextFormat normalText;

    sf::Color buttonColor,
        buttonHotColor,
        buttonPressedColor,
        buttonInactiveColor,
        textColor,
        textAreaForeground,
        textAreaBackground;
    // TODO: organise color usage
    float heatRate; // inverse of time it takes for a UI element to 'heat up'

    int smallFontSize, normalFontSize, largeFontSize;

    float pageMargins;
    sf::Color pageBgColor;

    ui::Button::Style normalButton;
    ui::TextEntry::Style normalTextEntry;
    ui::Checkbox::Style normalCheckbox;

    Base();
  } base;

  /**
   * Splash screen.
   */
  struct Splash {
    Splash();
  } splash;

  /**
   * Top-level interface, implemented by Client.
   */
  struct Menu {
    float unfocusedPageScale;

    // position offsets for various elements, see the schema in
    // media/source2d/board.png
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

    sf::Color pageUnderlayColor, statusFontColor;

    int descSize; // size of bits of text that describe things
    float pageDescMargin;

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
        localhostButtonPos;

    std::string tutorialButtonDesc,
        localhostButtonDesc;

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
   * The action-packed multiplayer client.
   */
  struct Multi {
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
        lobbyButtonPos, lobbyButtonSep;

    sf::Color playerSpecColor, playerJoinedColor;
    std::string readyButtonActiveDesc, readyButtonDeactiveDesc;

    ui::TextFormat messageLogText, playerListText;

    Multi(const Base &base);
  } multi;

  /**
   * Render settings for the game.
   */
  struct SkyRender {
    float
        rollAmount = 25, // how many degrees it rolls to either side
        rollSpeed = 2, // how quickly it rolls 2 * rollAmount
        flipSpeed = 2; // how quickly it flips orientation

    float spriteSize = 200;

    sf::FloatRect afterburnArea = {-70, -10, 70, 20};

    sf::FloatRect barArea = {-100, -100, 200, 30};
    sf::Color
        throttleStall = sf::Color(0, 0, 0, 100),
        throttle = sf::Color::Black,
        health = sf::Color::Green,
        energy = sf::Color::Blue;
    float deathRate = 0.5;

    SkyRender();
  } skyRender;

  Style();
};

static const Style style;

