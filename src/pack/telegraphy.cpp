#include "telegraphy.h"

namespace pk {

/**
 * Telegraph.
 */

Telegraph::Telegraph(unsigned short port) : port(port) {
  sock.bind(port);
}

void Telegraph::process() {

}

void Telegraph::transmit(const Transmission &transmission) {

}

std::vector<Reception> Telegraph::receive() {
  return std::vector<Reception>();
}


}
