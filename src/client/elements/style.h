/**
 * Style settings.
 */
#pragma once
#include "client/ui/control.h"
#include "client/ui/widgets/widgets.h"

const static struct Style {
  /**
   * Basic style consistencies.
   */
  struct Base {
    sf::Vector2f pageSize;

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

    sf::Vector2f debugChooserPos; // general tab
    sf::Vector2f nicknameChooserPos; // player tab
    /* stub */ // controls tab
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
        readyButtonPos,
        spectateButtonPos;

    sf::Color playerSpecColor, playerJoinedColor;
    std::string readyButtonActiveDesc, readyButtonDeactiveDesc;

    Multi(const Base &base);
  } multi;

  Style();
} style;
