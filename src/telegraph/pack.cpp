#include "pack.h"

namespace tg {

BoolPack::BoolPack() : Pack<bool>(
    [](PacketWriter &writer, const bool &value) {
      writer.writeBit(value);
    },
    [](PacketReader &reader, bool &value) {
      value = reader.readBit();
    }) { }
}
