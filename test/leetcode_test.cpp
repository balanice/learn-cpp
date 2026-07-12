#include "LeetCode.h"
#include <gtest/gtest.h>

TEST(LeetCodeTest, TwoSum) {
    LeetCode lc;
    std::vector<int> nums = {2, 7, 11, 15};
    int target = 9;
    std::vector<int> expected = {0, 1};
    std::vector<int> result = lc.twoSum(nums, target);
    EXPECT_EQ(result, expected);
}

TEST(LeetCodeTest, TwoSumNoSolution) {
    LeetCode lc;
    std::vector<int> nums = {1, 2, 3};
    int target = 7;
    std::vector<int> expected = {};
    std::vector<int> result = lc.twoSum(nums, target);
    EXPECT_EQ(result, expected);
}

TEST(LeetCodeTest, AddTwoNumbers) {
    LeetCode lc;

    ListNode* l1 = new ListNode(2, new ListNode(4, new ListNode(3)));
    ListNode* l2 = new ListNode(5, new ListNode(6, new ListNode(4)));

    ListNode* result = lc.addTwoNumbers(l1, l2);

    std::vector<int> expected_values = {7, 0, 8};
    for (int val : expected_values) {
        ASSERT_NE(result, nullptr);
        EXPECT_EQ(result->val, val);
        result = result->next;
    }
    EXPECT_EQ(result, nullptr);

    // Clean up memory
    delete l1->next->next;
    delete l1->next;
    delete l1;
    delete l2->next->next;
    delete l2->next;
    delete l2;
}