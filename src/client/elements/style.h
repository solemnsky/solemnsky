/**
 * Stylesheet with all the style values for everything in the client, so our
 * code can be a bit less cluttered with arbitrary definitions.
 */

#ifndef SOLEMNSKY_STYLE_H
#define SOLEMNSKY_STYLE_H

#include "client/ui/control.h"
#include "client/ui/widgets/widgets.h"

static struct Style {
  /**
   * Basic style consistencies.
   */
  struct Base {
    sf::Color buttonColor,
        buttonHotColor,
        buttonPressedColor,
        textColor;
    // TODO: organise color usage

    int smallFontSize, normalFontSize, largeFontSize, hugeFontSize;

    ui::Button::Style normalButton;
    ui::Button::Style greyedButton;

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
   * The welcoming home page!
   */
  struct Home {
    sf::Vector2f tutorialButtonPos{500, 300},
        localhostButtonPos{1100, 300};

    std::string tutorialButtonDesc = "start tutorial",
        localhostButtonDesc = "connect to localhost";

    Home(const Base &base);
  } home;

  Style();
} style;

#endif //SOLEMNSKY_STYLE_H
