#include "into/into.h"

#include <gtest/gtest.h>

#include <chrono>

using namespace std::chrono_literals;

TEST(Chrono, DurationLosslessUpcast) {
    // milliseconds -> microseconds is lossless, handled by implicit strategy.
    const auto r = into::cast<std::chrono::microseconds>(5ms);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 5000us);
}

TEST(Chrono, DurationDowncastExact) {
    const auto r = into::cast<std::chrono::milliseconds>(2000us);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 2ms);
}

TEST(Chrono, DurationDowncastLossy) {
    const auto r = into::cast<std::chrono::milliseconds>(2500us);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::unrepresentable);
}

TEST(Chrono, SecondsToMinutesExact) {
    const auto r = into::cast<std::chrono::minutes>(120s);
    ASSERT_TRUE(r);
    EXPECT_EQ(*r, 2min);
}

TEST(Chrono, SecondsToMinutesLossy) {
    const auto r = into::cast<std::chrono::minutes>(125s);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::unrepresentable);
}

TEST(Chrono, FloatDurationAllowed) {
    // Floating-point reps: precision loss is allowed (matches numeric strategy).
    using fsec = std::chrono::duration<double>;
    const auto r = into::cast<fsec>(2500us);
    ASSERT_TRUE(r);
    EXPECT_DOUBLE_EQ(r->count(), 0.0025);
}

TEST(Chrono, TimePointDowncastExact) {
    using sys_ms = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
    using sys_us = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;
    const sys_us src{2000us};
    const auto r = into::cast<sys_ms>(src);
    ASSERT_TRUE(r);
    EXPECT_EQ(r->time_since_epoch(), 2ms);
}

TEST(Chrono, TimePointDowncastLossy) {
    using sys_ms = std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds>;
    using sys_us = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;
    const sys_us src{2500us};
    const auto r = into::cast<sys_ms>(src);
    ASSERT_FALSE(r);
    EXPECT_EQ(r.error(), into::Error::unrepresentable);
}
