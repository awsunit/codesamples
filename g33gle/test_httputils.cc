/*
 * Copyright ©2019 Hannah C Tang.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Autumn Quarter 2019 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./HttpUtils.h"

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string>

#include "gtest/gtest.h"
#include "./test_suite.h"

using std::string;

namespace hw4 {

TEST(Test_HttpUtils, TestHttpUtilsURIDecode) {
  // Test out URIDecoding.
  string empty("");
  string plain("foo");
  string two("%74%77%6f");
  string twoupper("%74%77%6F");
  string nope("%16nope");
  string broken("%broken%1");
  string spacey("%20+blah blah");
  ASSERT_EQ(string(""), URIDecode(empty));
  ASSERT_EQ(string("foo"), URIDecode(plain));
  ASSERT_EQ(string("two"), URIDecode(two));
  ASSERT_EQ(string("two"), URIDecode(twoupper));
  ASSERT_EQ(string("%16nope"), URIDecode(nope));
  ASSERT_EQ(string("%broken%1"), URIDecode(broken));
  ASSERT_EQ(string("  blah blah"), URIDecode(spacey));
}

TEST(Test_HttpUtils, TestHttpUtilsURLParser) {
  // Test out URL parsing.
  string easy("/foo/bar");
  string tricky("/foo/bar?");
  string query("/foo/bar?foo=blah+blah");
  string many("/foo/bar?foo=bar&bam=baz");
  string manyshort("/foo/bar?foo=%22bar%22&bam=baz");

  URLParser p;
  p.Parse(easy);
  ASSERT_EQ("/foo/bar", p.path());
  ASSERT_EQ((unsigned) 0, p.args().size());

  p.Parse(tricky);
  ASSERT_EQ("/foo/bar", p.path());
  ASSERT_EQ((unsigned) 0, p.args().size());

  p.Parse(query);
  ASSERT_EQ("/foo/bar", p.path());
  ASSERT_EQ((unsigned) 1, p.args().size());
  ASSERT_EQ("blah blah", p.args()["foo"]);

  p.Parse(many);
  ASSERT_EQ("/foo/bar", p.path());
  ASSERT_EQ((unsigned) 2, p.args().size());
  ASSERT_EQ("bar", p.args()["foo"]);
  ASSERT_EQ("baz", p.args()["bam"]);

  p.Parse(manyshort);
  ASSERT_EQ("/foo/bar", p.path());
  ASSERT_EQ((unsigned) 2, p.args().size());
  ASSERT_EQ("\"bar\"", p.args()["foo"]);
  ASSERT_EQ("baz", p.args()["bam"]);

  HW4Environment::AddPoints(20);
}

TEST(Test_HttpUtils, TestHttpUtilsIsPathSafe) {
  string basedir = "test_files/ok";
  string file1_ok = "test_files/ok/./bar";
  string file2_ok = "test_files/ok/baz/../bar";
  string file3_ok = "test_files/ok/../ok/baz";
  string file4_bad = "test_files/ok/../bad";
  string file5_bad = "test_files/ok/./..";
  string file6_bad = "//etc/passwd";

  ASSERT_TRUE(IsPathSafe(basedir, file1_ok));
  ASSERT_TRUE(IsPathSafe(basedir, file2_ok));
  ASSERT_TRUE(IsPathSafe(basedir, file3_ok));
  ASSERT_FALSE(IsPathSafe(basedir, file4_bad));
  ASSERT_FALSE(IsPathSafe(basedir, file5_bad));
  ASSERT_FALSE(IsPathSafe(basedir, file6_bad));
  HW4Environment::AddPoints(20);
}

TEST(Test_HttpUtils, TestHttpUtilsWrappedReadWrite) {
  string filedata = "This is a test; this is only a test.\n";

  // Make sure the file we'll write/read is deleted.
  unlink("test_files/test.txt");

  // Open the file and write to it.
  int file_fd = open("test_files/test.txt",
                     O_RDWR | O_CREAT,
                     S_IRUSR | S_IWUSR);
  ASSERT_NE(-1, file_fd);
  ASSERT_EQ(static_cast<int>(filedata.size()),
            WrappedWrite(file_fd,
                         (unsigned char *) filedata.c_str(),
                         filedata.size()));
  close(file_fd);

  // Reopen the file and read it in.
  unsigned char readdata[64] = { 0 };
  file_fd = open("test_files/test.txt", O_RDONLY);
  ASSERT_NE(-1, file_fd);
  ASSERT_EQ(static_cast<int>(filedata.size()),
            WrappedRead(file_fd, readdata, filedata.size()));
  close(file_fd);
  string readstr((const char *) readdata);
  ASSERT_TRUE(readstr == filedata);

  // Delete the file.
  unlink("test_files/test.txt");
  HW4Environment::AddPoints(20);
}

}  // namespace hw4
