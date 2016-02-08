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

class Client; // we send callbacks to this

struct ClientShared {
private:
  Client *client;
public:
  ClientShared(Client *client);

  ui::AppState *appState;
  Settings settings;
  double uptime;

  /**
   * The Game.
   */
  std::unique_ptr<class Game> game;
  bool gameInFocus(); // the game exists and is in focus

  /**
   * UI methods.
   */
  void beginGame(std::unique_ptr<Game> &&game);
  void focusPage(const PageType type);
  void unfocusPage();
};

#endif //SOLEMNSKY_CLIENTSTATE_H
