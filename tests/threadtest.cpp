#include <gtest/gtest.h>
#include "util/threads.h"
#include <SFML/System.hpp>
#include "util/printer.h"

/**
 * Whatever std::thread implementation we're using works properly.
 */
class ThreadTest: public testing::Test {
 public:
  ThreadTest() { }

};

class AsyncClass {
 public:
  AsyncClass() { }

  void operationA() {
    sf::sleep(sf::milliseconds(20));

    resultLock.lock();
    result += "testing ";
    resultLock.unlock();
  }

  void operationB() {
    sf::sleep(sf::milliseconds(20));

    resultLock.lock();
    result += "threads";
    resultLock.unlock();
  }

  std::mutex resultLock;
  std::string result;

};

TEST_F(ThreadTest, BasicTest) {
  AsyncClass x;

  std::thread threadA([&]() { x.operationA(); });
  std::thread threadB([&]() { x.operationB(); });

  ASSERT_EQ(x.result, "");

  threadA.join();
  threadB.join();

  appLog(x.result);
  ASSERT_EQ(
      (x.result == "testing threads")
          or (x.result == "threadstesting "), true);

}

