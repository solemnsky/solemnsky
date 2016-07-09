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
  environment.joinWorker(); // joins the initialization

  ASSERT_EQ(environment.loadingIdle(), true);
  ASSERT_EQ(environment.loadingErrored(), false);

  // The map is loading, but graphics and scripts aren't.
  ASSERT_NE(environment.getMap(), nullptr);
  ASSERT_EQ(environment.getVisuals(), nullptr);
  ASSERT_EQ(environment.getMechanics(), nullptr);
}

/**
 * Certain issues cause non-fatal, gracefully handled errors.
 */
TEST_F(EnvironmentTest, ErrorTest) {
  sky::Environment environment("does-not-exist");
  environment.joinWorker();

  ASSERT_TRUE(environment.loadingIdle());
  ASSERT_TRUE(environment.loadingErrored());
  ASSERT_EQ(environment.getMap(), nullptr);
}

/**
 * We can load environments from files.
 */
TEST_F(EnvironmentTest, BasicTest) {
  // we should have a demo.sky in the environment directory
  sky::Environment environment("demo");
  ASSERT_FALSE(environment.loadingIdle());
  environment.joinWorker();
  // this might take a second, we need to unpack the archive and load the map

  // The map has loaded properly!
  ASSERT_TRUE(environment.loadingIdle());
  ASSERT_FALSE(environment.loadingErrored());
  ASSERT_NE(environment.getMap(), nullptr);

  ASSERT_EQ(environment.getMap()->getDimensions().x, 1600);
}

/**
 * Graphics can be loaded from environment files too, when we ask for them.
 */
TEST_F(EnvironmentTest, GraphicsTest) {
  sky::Environment environment("demo");
  environment.joinWorker();

  environment.loadMore(true, false);
  ASSERT_FALSE(environment.loadingIdle());
  environment.joinWorker();
  ASSERT_TRUE(environment.loadingIdle());

  ASSERT_TRUE(environment.getVisuals());
  ASSERT_FALSE(environment.getMechanics());

  // TODO: more
}

/**
 * We can even load scripts from environment files.
 */
TEST_F(EnvironmentTest, ScriptTest) {
  sky::Environment environment("demo");
  environment.joinWorker();

  environment.loadMore(false, true);
  ASSERT_FALSE(environment.loadingIdle());
  environment.joinWorker();
  ASSERT_TRUE(environment.loadingIdle());

  ASSERT_FALSE(environment.getVisuals());
  ASSERT_TRUE(environment.getMechanics());

  // TODO: more
}

