#include "telegraph/telegraph.h"
#include "util/methods.h"
#include "gtest/gtest.h"

class TelegraphFixture: public testing::Test {
 public:
  tg::UsageFlag flag;

};

/**
 * We can transmit and receive stuff from telegraphs okay.
 */
TEST_F(TelegraphFixture, TransmitReceive) {
  // bepbep
}
