#include "elements.h"

Game::Game(ClientShared &state,
           const std::string &name) :
    inFocus(true),
    concluded(false),
    state(state),
    name(name) { }