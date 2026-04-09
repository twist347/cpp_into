#include "into/into.h"

#include <gtest/gtest.h>

#include <string>
#include <string_view>

TEST(Implicit, CStringToString) {
    const char *s = "hello";
    const auto r = into::cast<std::string>(s);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, "hello");
}

TEST(Implicit, StringToStringView) {
    const std::string s = "hello";
    const auto r = into::cast<std::string_view>(s);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, "hello");
}
