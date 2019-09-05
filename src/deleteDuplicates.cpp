#include <stdio.h>
#include <stdlib.h>
#include "global.h"

#define TEST(x, y) printf("%12d --->> %-12d  %s\r\n", x, A.reverse(x), y==A.reverse(x)? "Successed":"Failed")
// 1 -> 1 -> 1 -> 4 ->5  ----->> 1->4-> 5
// 1 -> 2 -> 2 -> 5  1->2->5
//
class Solution
{
public:
    ListNode* deleteDuplicates(ListNode* head)
    {
        if(head == NULL) return NULL;
        ListNode *dummpy = new ListNode(0);
        dummpy->next = head;
        ListNode *node = head;

        while(node->next != NULL && node->next->next != NULL)
        {
            if(node->next->val == node->next->next->val)
            {
                int tmpVal = node->next->val;
                while(node->next != NULL && tmpVal == node->next->val)
                {
                    ListNode * tmpNode = node->next;
                    node->next = node->next->next;
                    delete tmpNode;
                }
            }
            else
            {
                node = node->next;
            }
            

        }
        return dummpy->next;
    }

};

