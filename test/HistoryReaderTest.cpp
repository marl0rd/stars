
#include <gtest/gtest.h>
#include "HistoryReader.hpp"

#include <filesystem>

TEST(HistoryReader, ReadsSampleFile) {
  const std::string path = std::string("../resources/.bash_history");
  ASSERT_TRUE(std::filesystem::exists(path));

  Stars::HistoryReader reader;
  auto entries = reader.readFile(path);

  ASSERT_EQ(entries.size(), 5u);
  EXPECT_EQ(entries[0].raw, "ls -la");
  EXPECT_EQ(entries[1].raw, "cd src");
}
