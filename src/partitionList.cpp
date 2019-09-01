#include <stdio.h>
#include <stdlib.h>
#include "global.h"
// 输入链表 x  小于x放在左边 大于x放在右边

class Solution
{
public:
    ListNode* partitionList(ListNode* head, int x)
    {
        if(head == NULL) return NULL;

        ListNode *leftdummpy = new ListNode(0);
        ListNode *rightdummpy = new ListNode(0);
        ListNode *left = leftdummpy;
        ListNode *right = rightdummpy;
        ListNode *node = head;

        while(node != NULL)
        {
            if (node->val < x)
            {
                left->next = node;
                left = left->next;
            }
            else
            {
                right->next = node;
                right = right->next;
            }

            right->next = NULL;
            left->next = rightdummpy ->next;
        }
        return leftdummpy->next;
    }

};

