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
  Archive archive("test.zip");
  ASSERT_EQ(archive.isDone(), false);

  archive.load();
  archive.finishLoading(); // blocking

  ASSERT_EQ(archive.isDone(), true);
  ASSERT_EQ((bool) archive.getError(), false);
  ASSERT_EQ((bool) archive.getResult(), true);

  const auto dir = *archive.getResult();
  ASSERT_EQ(dir.name, "test.zip");
  ASSERT_EQ(dir.files, std::vector<std::string>({"asdf", "fdsa"}));
  ASSERT_EQ(dir.directories.size(), size_t(1));

  const auto subdir = dir.directories[0];
  ASSERT_EQ(subdir.name, "subdirectory");
}

