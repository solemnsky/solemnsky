#include "serverlog.h"

namespace sky {

ServerLog::ServerLog(Sky *sky, std::function<void(std::string &&)> logger) :
    Subsystem(sky, (std::function<_Res(_ArgTypes...)>())) { }

void ServerLog::tick(const float delta) {

}

void ServerLog::addPlane(const PID pid, PlaneVital &plane) {

}

void ServerLog::removePlane(const PID pid) {

}

}
