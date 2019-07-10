#include <stdio.h>
#include <stdlib.h>
#include "global.h"

#define TEST(x, y) printf("%12d --->> %-12d  %s\r\n", x, A.reverse(x), y==A.reverse(x)? "Successed":"Failed")
// 1 -> 2 -> 3 -> 4 ->5
// 1 -> 2 -> 4 -> 5 删除倒数第二个
//
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
            if(q == NULL)return NULL;
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
    ListNode *listNode, *tmp, *prev=NULL;
    ListNode *p;

    for(int i = 0;i < 10;i++)
    {
        p = (ListNode *) malloc(sizeof(*p));  
        p->val = i + 10;
        p->next = NULL;
       // ListNode willInsert(i+10);
       // p = &willInsert;

        if(i == 0)
            listNode = p;
        if(prev != NULL)
            prev->next = p;
        prev = p;
    }
    //free(p);
    // for(int j = 0; j < 10;j++)
    // {
    //     free(p);
   //     p = p->next;
   // }
   
    printf("listNode->val =%d\n", listNode->val);
    tmp = listNode; 

    while(tmp != NULL)
    {
        printf("tmp->val =%d\n", tmp->val);
        tmp = tmp->next;
    }

    listNode = A.removeNthFromEnd(listNode, 2);
    
    while(listNode != NULL)
    {
        printf("after listNode->val =%d\n",listNode->val);
        listNode = listNode->next;
    }

    // if(1 == A.isSameTree(p, q))
     //   printf("is same tree\n");
   // else
     //   printf("is not same tree\n");
}
