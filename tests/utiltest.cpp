#include <vector>
#include <gtest/gtest.h>
#include "util/types.hpp"
#include "util/methods.hpp"

/**
 * The basic utilities we have in src/util.
 */
class UtilTest: public testing::Test {
 public:
  UtilTest() { }
};

TEST_F(UtilTest, RollingSamplerTest) {
  RollingSampler<int> sampler(3);
  EXPECT_EQ(sampler.min(), 0);
  EXPECT_EQ(sampler.max(), 0);
  EXPECT_EQ(sampler.mean<float>(), 0.0f);
  sampler.push(5);
  sampler.push(6);
  sampler.push(7);
  EXPECT_EQ(sampler.min(), 5);
  EXPECT_EQ(sampler.max(), 7);
  EXPECT_EQ(sampler.mean<float>(), 6.0f);
  sampler.push(8);
  EXPECT_EQ(sampler.min(), 6);
}

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
  EXPECT_EQ(x -= 0.7, 0);
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

TEST_F(UtilTest, AllocTest) {
  std::vector<PID> x = {3, 1, 0};
  EXPECT_EQ(smallestUnused(x), PID(2));
  x = {2, 1, 0};
  EXPECT_EQ(smallestUnused(x), PID(3));
  x = {2, 1, 3};
  EXPECT_EQ(smallestUnused(x), PID(0));
  x = {0, 1, 2, 3, 4};
  EXPECT_EQ(smallestUnused(x), PID(5));
}

/**
 * We can read stuff from strings.
 */
TEST_F(UtilTest, ReadTest) {
  // Floats!
  ASSERT_TRUE(bool(readFloat("1")));
  EXPECT_EQ(*readFloat("1"), 1);

  ASSERT_TRUE(bool(readFloat("42 ")));
  EXPECT_EQ(*readFloat("42 "), 42);

  ASSERT_TRUE(bool(readFloat(" 3.0 ")));
  EXPECT_EQ(*readFloat(" 3.0 "), 3.0);

  EXPECT_FALSE(bool(readFloat("a")));
  EXPECT_FALSE(bool(readFloat("1,")));
  EXPECT_FALSE(bool(readFloat("1a")));

  // Other things??
}
