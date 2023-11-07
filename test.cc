#include <gtest/gtest.h>

TEST (Sanity, SanityCheck)
{
    EXPECT_EQ(1, 1);
}

int main()
{
    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}