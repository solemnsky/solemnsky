#include "pack.h"

namespace tg {

BoolPack::BoolPack() : Pack<bool>(
    [](PacketWriter &writer, const bool &value) {
      writer.writeBit(value);
    },
    [](PacketReader &reader, bool &value) {
      value = reader.readBit();
    }) { }

StringPack::StringPack() : Pack<std::string>(
    [](PacketWriter &writer, const std::string &value) {
      for (const char x : value)
          writer.writeChar((unsigned char) x);
      writer.writeChar(0);
    },
    [](PacketReader &reader, std::string &value) {
      value.clear();
      unsigned char current;
      while ((current = reader.readChar()) != 0)
          value.push_back(current);
    }) { }
}
