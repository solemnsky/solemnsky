#include "engine/flowcontrol.hpp"
#include <gtest/gtest.h>

/**
 * FlowControl allows management of message streams with variable timeflow.
 */
class FlowTest: public testing::Test {
 public:
  FlowTest() : control() { }

  sky::FlowControl<size_t> control;

};

// Pseudorandom offset values for the simulation.
static std::vector<Time> offsets = {
  
};

/**
 * We simulate a connection and verify some statistical properties about the results.
 */
TEST_F(FlowTest, Simulation) {

  size_t i = 0;
  for (auto offset : offsets) {
    const Time localtime = i * 15;
    control.registerMessage(localtime, localtime + offsets[offset], i);
    // etc
  }
}
