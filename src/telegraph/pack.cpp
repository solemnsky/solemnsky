#include "pack.h"

namespace tg {

BoolPack::BoolPack() : Pack<bool>(
    [](Packet &packet, const bool &value) {
      packet.writeBit(value);
    },
    [](Packet &packet, bool &value) {
      value = packet.readBit();
    }) { }

}
