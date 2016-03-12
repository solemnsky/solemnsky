#include "util/types.h"
#include "gtest/gtest.h"

/**
 * The basic utilities we have in src/util.
 */
class UtilTest: public testing::Test {
 public:
  UtilTest() { }
};

TEST_F(UtilTest, CooldownTest) {
  Cooldown x{1};
  EXPECT_FALSE(x.cool(0.6));
  EXPECT_TRUE(x.cool(0.5));
  x.reset();
  EXPECT_FALSE((bool) x);
  x.prime();
  EXPECT_TRUE((bool) x);
}

TEST_F(UtilTest, ClampedTest) {
  Clamped x;
  EXPECT_EQ(x = 2, 1);
  EXPECT_EQ(x += 0.5, 1);
  EXPECT_EQ(x -= 0.4, 0.6f);
}

TEST_F(UtilTest, CyclicTest) {
  Cyclic x{-5, 5};
  EXPECT_EQ(x = -5, -5);
  EXPECT_EQ(x = 5, -5);
  EXPECT_EQ(x = 6, -4);
  EXPECT_EQ(x = -7, 3);
  EXPECT_EQ(cyclicDistance(x, -5), 2);
}

TEST_F(UtilTest, AngleTest) {
  Angle x;
  EXPECT_EQ(x = 360, 0);
  EXPECT_EQ(x = 400, 40);
  EXPECT_EQ(x = -10, 350);
}
