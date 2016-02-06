#include "telegraph/telegraph.h"
#include "gtest/gtest.h"

class TelegraphFixture : public testing::Test {
public:
  TelegraphFixture() { }

  ~TelegraphFixture() { }

  tg::Packet buffer;
  tg::Telegraph telegraph1{4242};
  tg::Telegraph telegraph2{4243};
};

/**
 * We can transmit and receive stuff from telegraphs okay.
 */
TEST_F(TelegraphFixture, TransmitReceive) {
  const tg::Pack<std::string> stringPack = tg::StringPack();

  const std::string message{"hello world"};
  tg::packInto(stringPack, message, buffer);
  telegraph1.transmit(
      tg::Transmission(buffer, sf::IpAddress("localhost"), 4243));

  std::string result;
  bool received = false;
  while (!received) {
    telegraph2.receive([&](tg::Reception &&reception){
      tg::unpackInto(stringPack, reception.packet, result);
      received = true;
    });
  }
  EXPECT_EQ(result, message);
}
