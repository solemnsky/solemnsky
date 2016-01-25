#include "server.h"
#include "util/packer.h"
#include "util/methods.h"

int main() {
  const PackRules<float> floatPack = ValueRules<float>::rules();
  const PackRules<optional<float>> optionalPack =
      OptionalRules<float>::rules(floatPack);

  Packet packet;
  packet = pack(floatPack, 999999.0f);
  packet.dump();
  appLog(LogType::Debug, std::to_string(unpack(floatPack, packet)));

  packet = pack(optionalPack, {88888.0f});
  packet.dump();
  appLog(LogType::Debug, std::to_string(*unpack(optionalPack, packet)));
}
