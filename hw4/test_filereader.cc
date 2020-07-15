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

#include "./FileReader.h"

#include "gtest/gtest.h"
#include "./test_suite.h"

using std::string;

namespace hw4 {

TEST(Test_FileReader, TestFileReaderBasic) {
  // See if we can read a file successfully.
  FileReader f(".", "test_files/hextext.txt");
  string contents;
  ASSERT_TRUE(f.ReadFile(&contents));
  ASSERT_EQ(4800U, contents.size());
  HW4Environment::AddPoints(10);

  // Make sure reading a non-existent file fails.
  f = FileReader(".", "non-existent");
  ASSERT_FALSE(f.ReadFile(&contents));
  HW4Environment::AddPoints(5);

  // Make sure a file path attack fails.
  f = FileReader("./libhw2", "./libhw2/../cpplint.py");
  ASSERT_FALSE(f.ReadFile(&contents));
  HW4Environment::AddPoints(5);
}

}  // namespace hw4
