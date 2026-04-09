#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

#include <gtest/gtest.h>

#include "into/into.h"

using IntStr = std::variant<int, std::string>;

TEST(Variant, IntoVariantPicksFirstAlternative) {
    const auto r = into::cast<IntStr>(42);
    ASSERT_TRUE(r);
    ASSERT_EQ(r->index(), 0u);
    EXPECT_EQ(std::get<int>(*r), 42);
}

TEST(Variant, IntoVariantFallsThroughToSecond) {
    // 300 doesn't fit into int8_t; should land in the std::string alternative.
    using V = std::variant<std::int8_t, std::string>;
    const auto r = into::cast<V>(300);
    ASSERT_TRUE(r);
    ASSERT_EQ(r->index(), 1u);
    EXPECT_EQ(std::get<std::string>(*r), "300");
}

TEST(Variant, IntoVariantStringViewParsesAsInt) {
    // string_view -> int via from_chars succeeds, so the int alternative wins
    // even though string_view -> std::string would also work.
    const auto r = into::cast<IntStr>(std::string_view{"42"});
    ASSERT_TRUE(r);
    ASSERT_EQ(r->index(), 0u);
    EXPECT_EQ(std::get<int>(*r), 42);
}

TEST(Variant, FromVariantToCommonType) {
    const IntStr v = 42;
    const auto r = into::cast<long>(v);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 42L);
}

TEST(Variant, FromVariantStringAlternativeToInt) {
    const IntStr v = std::string{"42"};
    const auto r = into::cast<int>(v);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 42);
}

TEST(Variant, FromVariantStringAlternativeInvalid) {
    const IntStr v = std::string{"not a number"};
    const auto r = into::cast<int>(v);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::invalid_format);
}

TEST(Variant, VariantToVariantNarrowing) {
    using V1 = std::variant<int, std::string>;
    using V2 = std::variant<long, std::string>;
    const V1 src = 42;
    const auto r = into::cast<V2>(src);
    ASSERT_TRUE(r);
    ASSERT_EQ(r->index(), 0u);
    EXPECT_EQ(std::get<long>(*r), 42L);
}

TEST(Variant, VariantToVariantStringAlternative) {
    using V1 = std::variant<int, std::string>;
    using V2 = std::variant<long, std::string>;
    const V1 src = std::string{"hello"};
    const auto r = into::cast<V2>(src);
    ASSERT_TRUE(r);
    ASSERT_EQ(r->index(), 1u);
    EXPECT_EQ(std::get<std::string>(*r), "hello");
}
