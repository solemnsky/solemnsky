/**
 * This is the shared core of the client, which all sub-elements refer to. It
 * has globally useful factors such as global settings, a unique_ptr to the Game
 * currently active, and UI methods that influence the whole Client.
 *
 * Try to keep this small, cross-cutting state is helpful in moderation.
 */
#ifndef SOLEMNSKY_CLIENTSTATE_H
#define SOLEMNSKY_CLIENTSTATE_H

#include "client/ui/control.h"
#include "settings.h"

// recursive relationship with Client: it instantiates ClientShared
class Client;

enum class PageType {
  Home, Listing, Settings
};

class ClientUiState {
private:
  const float pageFocusAnimSpeed = 3, // s^-1
      gameFocusAnimSpeed = 4;

  /**
   * Modifying
   */
  friend class Client;

  void focusGame();
  void blurGame();
  void focusPage(PageType page);
  void blurPage();

  void tick(float delta);

public:
  ClientUiState();

  PageType focusedPage;
  bool pageFocusing;
  bool gameFocusing;

  Clamped pageFocusFactor;
  Clamped gameFocusFactor;

  bool pageFocused() const;
  bool menuFocused() const;
  bool gameFocused() const;
};

struct ClientShared {
private:
  Client *client;
public:
  ClientShared(Client *client);

  ui::AppState *appState;
  Settings settings;
  double uptime;

  /**
   * The game.
   */
  std::unique_ptr<class Game> game;

  /**
   * UI state, read-only by elements.
  */
  ClientUiState ui;

  /**
   * UI methods, propogating through the whole Client.
   */
  void beginGame(std::unique_ptr<Game> &&game);
  void unfocusGame();
  void exitGame();

  void focusPage(const PageType type);
  void unfocusPage();

  void changeSettings(const SettingsDelta &settings);
};

#endif //SOLEMNSKY_CLIENTSTATE_H
