#include "server.h"
#include "util/packer.h"
#include "util/methods.h"

struct MyStruct {
  MyStruct() = default;

  MyStruct(float x, float y) : x(x), y(y) { };

  float x;
  float y;

  std::string show() {
    return "x: " + std::to_string(x) + " y: " + std::to_string(y);
  }
};

int main() {
  const PackRule<float> floatPack = ValueRule<float>::rules();
  const PackRule<optional<float>> optionalPack =
      OptionalRule<float>::rules(floatPack);
  const PackRule<MyStruct> classPack =
      ClassRule<MyStruct>::rules(
          MemberRule<MyStruct, float>(
              floatPack,
              [](const MyStruct parent) -> float { return parent.x; },
              [](MyStruct &parent, float x) { parent.x = x; })),
          MemberRule<MyStruct, float>(
              floatPack,
              [](const MyStruct parent) -> float { return parent.y; },
              [](MyStruct &parent, float y) { parent.y = y; })
      );

  Packet packet;

  appLog(LogType::Debug, "** floatPack **");
  float fl = 999999.0f;
  appLog(LogType::Debug, "input: " + std::to_string(fl));
  packet = pack(floatPack, fl);
  packet.dump();
  appLog(LogType::Debug, "output: " +
                         std::to_string(unpack(floatPack, packet)));

  appLog(LogType::Debug, "** optionalPack **");
  optional<float> optFl = {88888.0f};
  appLog(LogType::Debug, "input: " + std::to_string(*optFl));
  packet = pack(optionalPack, {88888.0f});
  packet.dump();
  appLog(LogType::Debug, "output: " +
                         std::to_string(*unpack(optionalPack, packet)));

  appLog(LogType::Debug, "** classPack **");
  MyStruct myStruct{42, 53};
  appLog(LogType::Debug, "input: " + myStruct.show());
  packet = pack(classPack, myStruct);
  packet.dump();
  appLog(LogType::Debug, "output: " + unpack(classPack, packet).show());
}
