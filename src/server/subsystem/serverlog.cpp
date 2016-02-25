#include "serverlog.h"

namespace sky {

ServerLog::ServerLog(Sky *sky, std::function<void(std::string &&)> logger) :
  Subsystem(sky) {

}

void ServerLog::tick(const float delta) {

}

void ServerLog::joinPlane(const PID pid, PlaneHandle &plane) {

}

void ServerLog::quitPlane(const PID pid) {

}

void ServerLog::spawnPlane(const PID pid, PlaneHandle &plane) {

}

void ServerLog::killPlane(const PID pid, PlaneHandle &plane) {

}

}
