/**
 * Subsystem that logs engine events to some place.
 */
#ifndef SOLEMNSKY_SERVERLOG_H
#define SOLEMNSKY_SERVERLOG_H

#include "sky/subsystem.h"
#include "util/types.h"

namespace sky {
class ServerLog : sky::Subsystem {
private:
  std::function<void(std::string &&)> logger;
public:
  ServerLog(std::function<void(std::string &&)> logger);
};
}

#endif //SOLEMNSKY_SERVERLOG_H
