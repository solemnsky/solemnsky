#include <gtest/gtest.h>
#include "util/printer.hpp"
#include "util/archive.hpp"

/**
 * Our small 7zip wrapper for reading zip archives
 * does what one might expect it to.
 */
class ArchiveTest : public testing::Test {
 public:
  ArchiveTest() {}

};

/**
 * Directory::open() lets us open and investigate directories.
 */
TEST_F(ArchiveTest, DirectoryTest) {
  // We can tell when a directory doesn't exist.
  ASSERT_EQ((bool) Directory::open("../../tests/definitely-not-a-test"), false);

  // We can tell when a directory exists.
  const auto dir = Directory::open("../../tests/test"); // The test directory.
  ASSERT_EQ(bool(dir), true);

  // We get its name.
  ASSERT_EQ(dir->name, "test");

  // And files.
  ASSERT_EQ(dir->files.size(), size_t(2));
  ASSERT_EQ(dir->files[0].filename(), "asdf");
  ASSERT_EQ(dir->files[1].filename(), "fdsa");

  // As well as subdirectories...
  ASSERT_EQ(dir->directories.size(), size_t(1));

  // Which have their own name and files.
  const auto &subdir = dir->directories[0];
  ASSERT_EQ(subdir.name, "subdirectory");
  ASSERT_EQ(subdir.files.size(), size_t(0));
}

/**
 * Archive lets us load archives asynchronously and get their contents as a Directory.
 */
TEST_F(ArchiveTest, BasicTest) {
//  Archive archive("test.zip"); // The test archive.
//  ASSERT_EQ(archive.isDone(), false);
//
//  archive.load();
//  archive.finishLoading(); // blocking
//
//  ASSERT_EQ(archive.isDone(), true);
//  ASSERT_EQ((bool) archive.getError(), false);
//  ASSERT_EQ((bool) archive.getResult(), true);
//
//  const auto dir = *archive.getResult();
//  ASSERT_EQ(dir.name, "test.zip");
//  ASSERT_EQ(dir.files.siz
//  ASSERT_EQ(dir.directories.size(), size_t(1));
//
//  const auto subdir = dir.directories[0];
//  ASSERT_EQ(subdir.name, "subdirectory");
}

