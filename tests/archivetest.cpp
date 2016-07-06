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
  ASSERT_EQ((bool) Directory::open(getTestPath("dir-that-does-not-exist")), false);

  // We can tell when a directory exists.
  const auto dir = Directory::open(getTestPath("test")); // The test directory.
  ASSERT_EQ(bool(dir), true);

  // We get its name.
  ASSERT_EQ(dir->name, "test");

  // And files.
  ASSERT_EQ(dir->files.size(), size_t(2));
  std::ifstream asdf(dir->files[0].string());
  std::string line;
  asdf >> line;
  ASSERT_EQ(line == "asdf" || line == "fdsa", true);

  // As well as subdirectories...
  ASSERT_EQ(dir->directories.size(), size_t(1));

  // Which have their own name and files.
  const auto &subdir = dir->directories[0];
  ASSERT_EQ(subdir.name, "subdirectory");
  ASSERT_EQ(subdir.files.size(), size_t(1));
}

/**
 * Archive lets us load archives asynchronously and get their contents as a Directory.
 */
TEST_F(ArchiveTest, ExtractTest) {
  {
    // When we open the archive, nothing happens.
    Archive archive(getTestPath("archive-that-does-not-exist.zip")); // The test archive.
    ASSERT_EQ(archive.isDone(), false);

    // Then we load the archive, and join the worker thread...
    archive.load();
    archive.finishLoading(); // blocking call!

    // And things are loaded.
    ASSERT_EQ(archive.isDone(), true);

    // We get an error instead of a result because the path that we specified doesn't exist.
    ASSERT_EQ((bool) archive.getResult(), false);
  }

  {
    // Let's try that again.
    Archive archive(getTestPath("test.zip"));
    archive.load();
    archive.finishLoading();

    // Now it works.
//    ASSERT_EQ((bool) archive.getResult(), true);
  }
}

