/**
 * The continuous state of a client, accessible and modifiable from any page.
 */
#ifndef SOLEMNSKY_CLIENTSTATE_H
#define SOLEMNSKY_CLIENTSTATE_H

#include "ui/ui.h"

class ClientState {

};

struct Settings {
public:
  Settings();
  Settings(std::string filepath);

  void readFromFile(std::string filepath);
  void writeToFile(std::string filepath);

  bool fullscreen;
  bool debugInfo;
};

#endif //SOLEMNSKY_CLIENTSTATE_H
