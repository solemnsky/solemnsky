#include "engine/environment/environment.hpp"
#include <gtest/gtest.h>

/**
 * Our sky::Environment structure supports reading from file and
 * asynchronous loading.
 */
class EnvironmentTest : public testing::Test {
 public:
  EnvironmentTest() {}

};

/**
 * The null environment works -- useful for testing and stuff.
 */
TEST_F(EnvironmentTest, NullTest) {
  sky::Environment environment("NULL"); // the magic null URL
  environment.waitForLoading(); // joins the initialization

  ASSERT_EQ(environment.loadingIdle(), true);
  ASSERT_EQ(environment.loadingErrored(), false);

  // The map is loading, but graphics and scripts aren't.
  ASSERT_NE(environment.getMap(), nullptr);
  ASSERT_EQ(environment.getGraphics(), nullptr);
  ASSERT_EQ(environment.getScripts(), nullptr);

}

