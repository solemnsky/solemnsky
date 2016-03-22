#include "subsystem.h"
#include "arena.h"

namespace sky {

Subsystem::Subsystem(Arena *arena) :
    arena(arena) { }


void Subsystem::onTick(const float delta) {

}

void Subsystem::onJoin(const class Player &player) {

}

void Subsystem::onQuit(const class Player &player) {

}

}
