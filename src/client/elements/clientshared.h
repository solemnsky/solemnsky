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

enum class PageType {
  Home, Listing, Settings
};

// we send callbacks to the Client, but it instantiates ClientShared
class Client;

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
  bool gameFocused;

  /**
   * UI methods, propogating through the whole Client.
   */
  void beginGame(std::unique_ptr<Game> &&game);
  void unfocusGame();
  void focusPage(const PageType type);
  void unfocusPage();
};

#endif //SOLEMNSKY_CLIENTSTATE_H
