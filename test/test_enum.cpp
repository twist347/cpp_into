#include "into/into.h"

#include <gtest/gtest.h>

#include <cstdint>

namespace {
    enum class Color : std::uint8_t {
        red = 1,
        green = 2,
        blue = 200,
    };

    enum class Tiny : std::int8_t {
        a = -1,
        b = 0,
        c = 1,
    };
}

TEST(Enum, EnumToInt) {
    const auto r = into::cast<int>(Color::blue);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 200);
}

TEST(Enum, EnumToNarrowIntOverflow) {
    const auto r = into::cast<std::int8_t>(Color::blue);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Enum, IntToEnumInRange) {
    const auto r = into::cast<Color>(2);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, Color::green);
}

TEST(Enum, IntToEnumOutOfRange) {
    const auto r = into::cast<Color>(300);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Enum, IntToEnumNegativeIntoUnsigned) {
    const auto r = into::cast<Color>(-1);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Enum, EnumToEnumInRange) {
    const auto r = into::cast<Tiny>(Color::red);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, Tiny::c);
}

TEST(Enum, EnumToEnumOutOfRange) {
    const auto r = into::cast<Tiny>(Color::blue);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Enum, FloatToEnumFractional) {
    const auto r = into::cast<Color>(2.5);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::unrepresentable);
}
