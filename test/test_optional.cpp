#include "into/into.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <optional>

TEST(Optional, WrapValue) {
    const auto r = into::cast<std::optional<int> >(42);
    ASSERT_TRUE(r);
    ASSERT_TRUE(r->has_value());
    EXPECT_EQ(**r, 42);
}

TEST(Optional, UnwrapValue) {
    const std::optional<int> opt = 42;
    const auto r = into::cast<int>(opt);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 42);
}

TEST(Optional, UnwrapEmpty) {
    const std::optional<int> opt;
    const auto r = into::cast<int>(opt);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::unrepresentable);
}

TEST(Optional, OptionalToOptionalWidening) {
    const std::optional<int> src = 42;
    const auto r = into::cast<std::optional<long> >(src);
    ASSERT_TRUE(r);
    ASSERT_TRUE(r->has_value());
    EXPECT_EQ(**r, 42L);
}

TEST(Optional, OptionalToOptionalEmpty) {
    const std::optional<int> src;
    const auto r = into::cast<std::optional<long> >(src);
    ASSERT_TRUE(r);
    EXPECT_FALSE(r->has_value());
}

TEST(Optional, OptionalToOptionalNarrowingOverflow) {
    const std::optional<int> src = 300;
    const auto r = into::cast<std::optional<std::int8_t> >(src);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Optional, OptionalStringToOptionalInt) {
    const std::optional<std::string_view> src{"42"};
    const auto r = into::cast<std::optional<int> >(src);
    ASSERT_TRUE(r);
    ASSERT_TRUE(r->has_value());
    EXPECT_EQ(**r, 42);
}
