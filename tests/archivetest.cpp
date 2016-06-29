#include <gtest/gtest.h>
#include <archive.h>
#include <archive_entry.h>
#include "util/printer.hpp"

/**
 * Our small libarchive wrapper for reading .tar files 
 * does what one might expect it to.
 */
class ArchiveTest: public testing::Test {
 public:
  ArchiveTest() { }

};

/**
 * LibArchive does what I expect it to.
 */
TEST_F(ArchiveTest, LibArchiveTest) {
  archive *archive;
  archive_entry *entry;
  char* buffer = new char[100];

  // Allocate the archive.
  archive = archive_read_new();
  archive_read_support_filter_all(archive);
  archive_read_support_format_all(archive);

  // Open the archive.
  ASSERT_EQ(
      archive_read_open_filename(archive, "tests/test.tar", 10240), 
      ARCHIVE_OK);

  // Loop over header entries.
  while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
    appLog("reading: " + std::string(archive_entry_pathname(entry)));
    size_t bufferSize, readSize = 0;
    bool isFirstLoop = true;

    // Incrementally read data.
    for (;;) {
      bufferSize = archive_read_data(archive, buffer, 100);
      if (bufferSize == 0) break;
      readSize += bufferSize;
      if (isFirstLoop) {
        appLog("contents of first buffer (literal): \n" + std::string(buffer, bufferSize));
      }
      isFirstLoop = false;
    }
    appLog("done! total read bytes: " + std::to_string(readSize));
  }

  // Free the archive.
  ASSERT_EQ(archive_read_free(archive), ARCHIVE_OK);
}

