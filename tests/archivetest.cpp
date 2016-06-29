#include <gtest/gtest.h>
#include <archive.h>
#include <archive_entry.h>
#include "util/printer.hpp"

/**
 * libarchive does what I expect it to.
 */
class ArchiveTest: public testing::Test {
 public:
  ArchiveTest() { }

};

/**
 * Basic things work.
 */
TEST_F(ArchiveTest, CopyTest) {
  archive *a;
  archive_entry *entry;
  char* buff = new char[1000];
  int r;

  a = archive_read_new();
  archive_read_support_filter_all(a);
  archive_read_support_format_all(a);

  r = archive_read_open_filename(a, "tests/test.tar", 10240); 
  ASSERT_EQ(r, ARCHIVE_OK);

  while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
    appLog("reading: " + std::string(archive_entry_pathname(entry)));
    size_t read_len;
    for (;;) {
      read_len = archive_read_data(a, buff, 1000);
      if (read_len == 0) break;
      if (buff) {
        appLog("read data: " + std::string(buff, read_len));
      } 
    }
    appLog("done reading entry");
  }

  r = archive_read_free(a);
  ASSERT_EQ(r, ARCHIVE_OK);
}

