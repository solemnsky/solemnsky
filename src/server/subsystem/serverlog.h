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
  virtual void joinPlane(const PID pid, PlaneHandle &plane) override;
  virtual void quitPlane(const PID pid) override;
  virtual void spawnPlane(const PID pid, PlaneHandle &plane) override;
  virtual void killPlane(const PID pid, PlaneHandle &plane) override;
};
}

#endif //SOLEMNSKY_SERVERLOG_H
