#include "telegraph/telegraph.h"
#include "util/methods.h"
#include "gtest/gtest.h"

const tg::Pack<std::string> stringPack = tg::StringPack();

class TelegraphFixture : public testing::Test {
public:
  tg::Packet buffer;
  tg::Telegraph<std::string, std::string> telegraph1, telegraph2;

  TelegraphFixture() :
    telegraph1(4242, stringPack, stringPack),
    telegraph2(4243, stringPack, stringPack) { }

  ~TelegraphFixture() { }
};

/**
 * We can transmit and receive stuff from telegraphs okay.
 */
TEST_F(TelegraphFixture, TransmitReceive) {
  const std::string message{"hello world"};

  // transmit
  telegraph1.transmit(message, {"localhost"}, 4243);

  // receive
  std::string result;
  bool received = false;
  while (!received) {
    telegraph2.receive([&](tg::Reception<std::string> &&reception) {
      result = reception.value;
      received = true;
    });
  }
  EXPECT_EQ(result, message);
}
