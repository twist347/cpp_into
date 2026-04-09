#include "into/into.h"

#include <gtest/gtest.h>

TEST(Error, ToString) {
    EXPECT_EQ(into::to_string(into::Error::invalid_format), "invalid_format");
    EXPECT_EQ(into::to_string(into::Error::out_of_range), "out_of_range");
    EXPECT_EQ(into::to_string(into::Error::partial_parse), "partial_parse");
    EXPECT_EQ(into::to_string(into::Error::unrepresentable), "unrepresentable");
    EXPECT_EQ(into::to_string(into::Error::unsupported), "unsupported");
}
