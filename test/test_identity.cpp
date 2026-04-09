#include "into/into.h"

#include <gtest/gtest.h>

#include <string>

TEST(Identity, Int) {
    const auto r = into::cast<int>(42);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 42);
}

TEST(Identity, String) {
    const std::string s = "hello";
    const auto r = into::cast<std::string>(s);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, "hello");
}
