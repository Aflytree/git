#include <stdio.h>
#include <stdlib.h>
#include "global.h"

#define TEST(x, y) printf("%12d --->> %-12d  %s\r\n", x, A.reverse(x), y==A.reverse(x)? "Successed":"Failed")
// 1 -> 2 -> 3 -> 4 ->5
// 1 -> 2 -> 4 -> 5 删除倒数第二个
//
class Solution
{
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

    // 1 -> 2 ->5
    // 2 -> 3 ->6
    //    ||
    //    \/
    // 1 -> 2 -> 2 -> 3 -> 6

    ListNode* mergeTwoLists(ListNode* l1, ListNode* l2)
    {
        ListNode dummpy(100);
        ListNode *p1 = l1,*p2 = l2;

        dummpy.next = p1;
        ListNode*prev = &dummpy;
        printf("prev->value =%d\n", prev->val);

        while(p1 && p2)
        {
            if(p1->val < p2->val)
            {
                prev = p1;
                p1 = p1->next;
            }
            else
            {
                prev->next = p2;
                p2 = p2->next;
               
                prev = prev->next;
                prev->next = p1;
            }
        }

        if(p2)
        {
            prev->next = p2;
        }
        
        if(dummpy.next == NULL)
            printf("dummpy.next is null \n");

        return dummpy.next;     
    }
};

void insertNodeToList(ListNode **head, int value)
{
    ListNode *newNode;
    newNode = (ListNode*)malloc(sizeof(ListNode *)); 
    newNode->val = value;
    newNode->next = NULL;
    
    if(*head == NULL)
    {
        *head = newNode;
        return ;
    }
    
    ListNode *iter = *head;
   
    while(iter->next != NULL)
    {
        iter = iter->next;
    }
   
    iter->next = newNode;
    return;
}


void mergeTwoListsTest()
{
    Solution A;
    ListNode *listNode, *prev=NULL;
    ListNode *l1 = NULL, *l2 = NULL;
    ListNode *retList;
    int value = 10;
    int value_ = 11;
    
    printf("time to begin insert\n");
    //添加元素
    for(int i = 0; i < 10;i++)
    {
        insertNodeToList(&l1, i);
        insertNodeToList(&l2, 20 + i);
    }
    ListNode *tmp = l1; 

    while(tmp != NULL)
    {
        printf("l1->val =%d\n", tmp->val);
        tmp = tmp->next;
    }
    
    tmp = l2; 
    while(tmp != NULL)
    {
        printf("l2->val =%d\n", tmp->val);
        tmp = tmp->next;
    }
    
    //添加元素
    //合并
    retList = A.mergeTwoLists(l2, l1);
    printf("after merge\n");

    while(retList != NULL)
    {
        printf("retList->val =%d\n", retList->val);
        retList = retList->next;
    }

}

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
}
