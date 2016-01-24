#include "server.h"
#include "util/packer.h"
#include "util/methods.h"

int main() {
  PackRules<float> floatPack = PackFloat::packer();
  PackRules<optional<float>> optPack = PackOptional<float>::packer(floatPack);

  Packet packet = pack(floatPack, 5.0f);
  packet.dump();
  appLog(LogType::Debug, std::to_string(unpack(floatPack, packet)));


//  packet = pack(optionalFloat, optional<float>(5.0f));
//  packet.dump();
//  appLog(LogType::Debug, std::to_string(
//      *unpack(optionalFloat, packet)));

}
