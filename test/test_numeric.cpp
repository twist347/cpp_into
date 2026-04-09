#include "into/into.h"

#include <gtest/gtest.h>

#include <cstdint>

TEST(Numeric, IntToIntInRange) {
    const auto r = into::cast<std::int8_t>(100);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 100);
}

TEST(Numeric, IntToIntOverflow) {
    const auto r = into::cast<std::int8_t>(300);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Numeric, SignedToUnsignedNegative) {
    const auto r = into::cast<unsigned>(-1);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Numeric, UnsignedToSignedOverflow) {
    const auto r = into::cast<std::int8_t>(static_cast<unsigned>(200));
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Numeric, FloatToIntExact) {
    const auto r = into::cast<int>(3.0);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 3);
}

TEST(Numeric, FloatToIntFractional) {
    const auto r = into::cast<int>(3.14);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::unrepresentable);
}

TEST(Numeric, FloatToIntNaN) {
    const auto r = into::cast<int>(std::nan(""));
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::unrepresentable);
}

TEST(Numeric, IntToFloat) {
    const auto r = into::cast<double>(42);
    ASSERT_TRUE(r);
    EXPECT_DOUBLE_EQ(*r, 42.0);
}

TEST(Numeric, FloatToFloat) {
    const auto r = into::cast<float>(3.14);
    ASSERT_TRUE(r);
    EXPECT_FLOAT_EQ(*r, 3.14f);
}
