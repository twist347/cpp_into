#include <cstdint>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

#include <gtest/gtest.h>

#include "into/into.h"

TEST(Tuple, TupleToTupleWidening) {
    const std::tuple<int, int> src{1, 2};
    const auto r = into::cast<std::tuple<long, long> >(src);
    ASSERT_TRUE(r);
    EXPECT_EQ(std::get<0>(*r), 1L);
    EXPECT_EQ(std::get<1>(*r), 2L);
}

TEST(Tuple, TupleToTupleHeterogeneous) {
    const std::tuple<int, std::string_view> src{42, "hi"};
    const auto r = into::cast<std::tuple<long, std::string> >(src);
    ASSERT_TRUE(r);
    EXPECT_EQ(std::get<0>(*r), 42L);
    EXPECT_EQ(std::get<1>(*r), "hi");
}

TEST(Tuple, TupleToTupleErrorPropagates) {
    const std::tuple<int, std::string_view> src{300, "nope"};
    const auto r = into::cast<std::tuple<std::int8_t, int> >(src);
    ASSERT_FALSE(r);
    // 300 doesn't fit int8_t -> out_of_range; "nope" -> invalid_format.
    // The first failing element wins.
    EXPECT_EQ(r.error(), into::Error::out_of_range);
}

TEST(Tuple, PairToPair) {
    const std::pair<int, std::string_view> src{42, "7"};
    const auto r = into::cast<std::pair<long, int> >(src);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->first, 42L);
    EXPECT_EQ(r->second, 7);
}

TEST(Tuple, PairToTuple) {
    const std::pair<int, int> src{1, 2};
    const auto r = into::cast<std::tuple<long, long> >(src);
    ASSERT_TRUE(r);
    EXPECT_EQ(std::get<0>(*r), 1L);
    EXPECT_EQ(std::get<1>(*r), 2L);
}

TEST(Tuple, TupleToPair) {
    const std::tuple<int, int> src{1, 2};
    const auto r = into::cast<std::pair<long, long> >(src);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->first, 1L);
    EXPECT_EQ(r->second, 2L);
}

TEST(Tuple, ThreeAryTuple) {
    const std::tuple<int, int, std::string_view> src{1, 2, "3"};
    const auto r = into::cast<std::tuple<long, long, int> >(src);
    ASSERT_TRUE(r);
    EXPECT_EQ(std::get<0>(*r), 1L);
    EXPECT_EQ(std::get<1>(*r), 2L);
    EXPECT_EQ(std::get<2>(*r), 3);
}
