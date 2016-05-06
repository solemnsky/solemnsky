#include "debuginfo.h"

namespace sky {

/**
 * DebugInfo.
 */

void DebugInfo::registerPlayer(Player &player) {

}

void DebugInfo::unregisterPlayer(Player &player) {

}

void DebugInfo::onTick(const float delta) {
  Subsystem::onTick(delta);
}

DebugInfo::DebugInfo(Arena &arena,
                     class Sky &sky,
                     const optional<PID> &player) :
    Subsystem(arena), sky(sky), player(player) { }

void DebugInfo::printReport(Printer &p) const {
  p.print("things are probably okay?");
}

}
