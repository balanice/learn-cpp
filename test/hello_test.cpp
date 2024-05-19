#include "../include/sort.h"
#include <gtest/gtest.h>

TEST(HelloTest, BasicAssertions)
{
    EXPECT_STRNE("hello", "world");
    EXPECT_EQ(7*6, 42);

    int arr[3] = {1, 3, 2};
    BubbleSort(arr, 3);
    EXPECT_EQ(arr[2], 3);
}