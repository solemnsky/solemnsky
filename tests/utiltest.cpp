#include "util/types.h"
#include "gtest/gtest.h"

/**
 * I know this looks stupid but hey, at least now we're *sure* we won't have
 * stupid bugs later. Further justifying this, I cite that I'm interested in
 * poking at GoogleTest a bit.
 */

TEST(UtilTest, CooldownTest) {
  Cooldown x{1};
  EXPECT_FALSE(x.cool(0.6));
  EXPECT_TRUE(x.cool(0.5));
  x.reset();
  EXPECT_FALSE((bool) x);
  x.prime();
  EXPECT_TRUE((bool) x);
}

TEST(UtilTest, ClampedTest) {
  Clamped x{-1, 1};
  EXPECT_EQ(x = 2, 1);
  EXPECT_EQ(x = 0, 0);
  EXPECT_EQ(x = -5, -1);
}

TEST(UtilTest, CyclicTest) {
  Cyclic x{-5, 5};
  EXPECT_EQ(x = -5, -5);
  EXPECT_EQ(x = 5, -5);
  EXPECT_EQ(x = 6, -4);
  EXPECT_EQ(x = -7, 3);
}

TEST(UtilTest, SwitchedTest) {
  Switched x{{0, 3, 5}, 5};
  EXPECT_ANY_THROW(x = 1);
  EXPECT_NO_THROW(x = 0);
  EXPECT_NO_THROW(x = 5);
}

TEST(UtilTest, AngleTest) {
  Angle x;
  EXPECT_EQ(x = 360, 0);
  EXPECT_EQ(x = 400, 40);
  EXPECT_EQ(x = -10, 350);
}

