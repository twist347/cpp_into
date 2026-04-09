#include "into/into.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <string>
#include <string_view>

TEST(String, ParseInt) {
    const auto r = into::cast<int>(std::string_view{"42"});
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 42);
}

TEST(String, ParseIntPartial) {
    const auto r = into::cast<int>(std::string_view{"42abc"});
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::partial_parse);
}

TEST(String, ParseIntInvalid) {
    const auto r = into::cast<int>(std::string_view{"oops"});
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::invalid_format);
}

TEST(String, ParseIntOverflow) {
    const auto r = into::cast<std::int8_t>(std::string_view{"9999"});
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(String, ParseFromStdString) {
    const std::string s = "3.14";
    const auto r = into::cast<double>(s);
    ASSERT_TRUE(r);
    EXPECT_DOUBLE_EQ(*r, 3.14);
}

TEST(String, IntToString) {
    const auto r = into::cast<std::string>(42);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, "42");
}

TEST(String, DoubleToString) {
    const auto r = into::cast<std::string>(3.5);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, "3.5");
}
