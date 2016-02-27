/**
 * Subsystem that logs engine events to some place.
 */
#ifndef SOLEMNSKY_SERVERLOG_H
#define SOLEMNSKY_SERVERLOG_H

#include "sky/sky.h"
#include "util/types.h"

namespace sky {
class ServerLog : sky::Subsystem {
private:
  std::function<void(std::string &&)> logger;
public:
  ServerLog(Sky *sky, std::function<void(std::string &&)> logger);

 private:
  virtual void tick(const float delta) override;
  virtual void addPlane(const PID pid, Plane &plane) override;
  virtual void removePlane(const PID pid) override;
};
}

#endif //SOLEMNSKY_SERVERLOG_H
