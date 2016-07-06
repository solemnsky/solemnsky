#include <gtest/gtest.h>
#include "util/printer.hpp"
#include "util/archive.hpp"

/**
 * Our small 7zip wrapper for reading zip archives
 * does what one might expect it to.
 */
class ArchiveTest: public testing::Test {
 public:
  ArchiveTest() { }

};

/**
 * Basic operations work.
 */
TEST_F(ArchiveTest, BasicTest) {

}

