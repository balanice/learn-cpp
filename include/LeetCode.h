#ifndef LEETCODE_H
#define LEETCODE_H

#include <vector>

struct ListNode
{
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class LeetCode
{
public:
    LeetCode() = default;
    ~LeetCode() = default;
    std::vector<int> twoSum(std::vector<int> &nums, int target);

    ListNode* addTwoNumbers(ListNode* l1, ListNode* l2);
};

#endif // LEETCODE_H