#include "telegraph/telegraph.h"
#include "util/methods.h"
#include "gtest/gtest.h"

const tg::Pack<std::string> stringPack = tg::StringPack();

class TelegraphFixture : public testing::Test {
public:
  TelegraphFixture() { }

  ~TelegraphFixture() { }

  tg::Packet buffer;
  tg::Telegraph<std::string, std::string>
      telegraph1{4242, stringPack, stringPack};
  tg::Telegraph<std::string, std::string>
      telegraph2{4243, stringPack, stringPack};
};

/**
 * We can transmit and receive stuff from telegraphs okay.
 */
TEST_F(TelegraphFixture, TransmitReceive) {
  const tg::Pack<std::string> stringPack = tg::StringPack();

  const std::string message{"hello world"};

  telegraph1.transmit(
      tg::Transmission<std::string>(message, {"localhost"}, 4243));

  std::string result;
  bool received = false;
  while (!received) {
    appLog(LogType::Debug, "trying...");
    telegraph2.receive([&](tg::Reception<std::string> &&reception) {
      result = reception.value;
      received = true;
    });
  }
  EXPECT_EQ(result, message);
}
