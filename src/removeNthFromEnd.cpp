#include <stdio.h>
#include "global.h"

#define TEST(x, y) printf("%12d --->> %-12d  %s\r\n", x, A.reverse(x), y==A.reverse(x)? "Successed":"Failed")


class Solution {
public:
    ListNode* removeNthFromEnd(ListNode* head, int n)
    {
        if(head == NULL || n<=0)
            return NULL;
        ListNode tmpHead(0);
        tmpHead.next = head;
        head = &tmpHead;

        ListNode*p,*q;
        p = q = head;
        
        for(int i = 0;i < n;i++)
        {
            if(p == NULL)return NULL;
            q = q->next; 
        }

        while(q->next != NULL)
        {
            q = q->next;
            p = p->next;
        }
        
        p->next=p->next->next;
        return head->next;
    
    }
};

void removeNthFromEndTest()
{
    Solution A;
   // if(1 == A.isSameTree(p, q))
     //   printf("is same tree\n");
   // else
     //   printf("is not same tree\n");
}
