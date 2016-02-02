#include "pack.h"

namespace pk {

BoolPack::BoolPack() : Pack<bool>(
    [](Packet &packet, const bool &value) {
      packet.writeBit(value);
    },
    [](Packet &packet, bool &value) {
      value = packet.readBit();
    }) { }

}
