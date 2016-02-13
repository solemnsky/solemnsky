#include "elements.h"

Game::Game(ClientShared &shared,
           const std::string &name) :
    shared(shared),
    concluded(false),
    name(name) { }
