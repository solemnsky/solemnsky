#include "engine/flowcontrol.hpp"
#include <gtest/gtest.h>

/**
 * FlowControl allows management of message streams with variable timeflow.
 */
class FlowTest : public testing::Test {
 public:
  FlowTest() : control({}) {}

  sky::FlowControl<std::string> control;

};

/**
 * Verify some basic properties.
 */
TEST_F(FlowTest, BasicTest) {
  // we get a timestamped message "hello" with timestamp of 0
  control.push(0, "hello");

  // presently there is no buffer
  {
    const optional<std::string> msg = control.pull(10);
    ASSERT_TRUE(bool(msg));
    ASSERT_EQ(*msg, "hello");
  }

  //

}

/**
 * Test FlowControl's capacity to handle long, noisy message streams.
 */
TEST_F(FlowTest, StressTest) {

}
