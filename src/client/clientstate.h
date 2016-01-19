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
  Settings(std::__cxx11::string filepath);

  void readFromFile(std::__cxx11::string filepath);
  void writeToFile(std::__cxx11::string filepath);

  bool fullscreen;
  bool debugInfo;
};

#endif //SOLEMNSKY_CLIENTSTATE_H
