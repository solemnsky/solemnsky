#include <gtest/gtest.h>
#include "util/threads.h"
#include <SFML/System.hpp>

/**
 * Whatever std::thread implementation we're using works properly.
 */
class ThreadTest: public testing::Test {
 public:
  ThreadTest() { }

};

class AsyncClass {
 public:
  AsyncClass() :
      done(false) { }

  void doThing() {
    sf::sleep(sf::milliseconds(20));
    done = true;
  }

  bool done;
};

TEST_F(ThreadTest, BasicTest) {
  AsyncClass x;
  std::thread myThread(x.doThing);
  ASSERT_EQ(x.done, false);
  myThread.join();
  ASSERT_EQ(x.done, true);
}

