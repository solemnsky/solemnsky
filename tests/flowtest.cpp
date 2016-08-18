#include "engine/flowcontrol.hpp"
#include <gtest/gtest.h>

/**
 * FlowControl allows management of message streams with variable timeflow.
 */
class FlowTest: public testing::Test {
 public:
  FlowTest() : control({}) { }

  sky::FlowControl<std::string> control;

};

/**
 * Verify some basic properties.
 */
TEST_F(FlowTest, BasicTest) {
  // We get a timestamped message "hello" with timestamp of 0.

  // By default, everything passes through.
  {
    control.push(0, "hello");
    const auto msg = control.pull(10);
    ASSERT_TRUE(bool(msg));
    ASSERT_EQ(*msg, "hello");
    ASSERT_FALSE(bool(control.pull(10)));
  }

  // Set the window entry to 10, so we only start releasing messages
  // when their timestamp is 10 seconds older than localtime.
  // The window is 1 second wide.
  control.settings.windowEntry = 10;
  control.settings.windowSize = 1;

  {
    control.push(10, "hello");
    ASSERT_FALSE(bool(control.pull(10)));
    ASSERT_FALSE(bool(control.pull(22)));
    ASSERT_TRUE(bool(control.pull(20.5)));
    ASSERT_FALSE(bool(control.pull(20.5)));
  }
}
