//
// Created by Chris on 2/5/2016.
//

#include "serverlog.h"

namespace sky {

ServerLog::ServerLog(Sky *sky, std::function<void(std::string &&)> logger) :
  Subsystem(sky) {

}

}
