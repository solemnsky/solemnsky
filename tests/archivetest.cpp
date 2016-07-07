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

  std::string asdfFile;
  for (const auto file : dir->files) {
    if (file.filename() == "asdf")
      asdfFile = file.string();
  }
  ASSERT_NE(asdfFile, "");

  // .. Which point to valid filesystem paths.
  std::ifstream asdf(asdfFile);
  std::string line;
  asdf >> line;
  ASSERT_EQ(line, "asdf");

  // We also have subdirectories...
  ASSERT_EQ(dir->directories.size(), size_t(1));

  // Which also contain files and subdirectories.
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
    archive.load(); // blocking call

    // And things are loaded.
    ASSERT_EQ(archive.isDone(), true);

    // We get an error instead of a result because the path that we specified doesn't exist.
    ASSERT_EQ((bool) archive.getResult(), false);
  }

  {
    // Let's try that again.
    Archive archive(getTestPath("test.zip"));
    archive.load();

    // Now it works.
   ASSERT_EQ((bool) archive.getResult(), true);
   const Directory &dir = *archive.getResult();
   ASSERT_EQ(dir.name, "test.zip");
   ASSERT_EQ(dir.files.size(), size_t(2));
  }
}

