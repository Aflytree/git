#include <iostream>
#include <stdio.h>
#include "global.h"
//ABDH##I##E##CF#J##G##

using namespace std;
#define TEST(x, y) printf("%12d --->> %-12d  %s\r\n", x, A.reverse(x), y==A.reverse(x)? "Successed":"Failed")


class Solution {
public:
    int isSameTree(TreeNode* p, TreeNode* q)
    {
        printf("p->value =%d\n", p->val);    
        printf("q->value =%d\n", q->val);    
       
        if(p->val != q->val)
            return 0;
        else
            return 1;
        return 0;
    }

    void CreateBiTree(TreeNode* &T)
    {
        char ch;
        char cb;
        
        printf("please input your value:\n");
        if((ch=getchar())=='#')
        {
            T=NULL;//其中getchar（）为逐个读入标准库函数
        }
        else
        {
            T = new TreeNode(ch);//产生新的子树
            printf("node val = %d\n",T->val);
            printf("T->left:\n");
            CreateBiTree(T->left);//递归创建左子树
            printf("T->right:\n");
            CreateBiTree(T->right);//递归创建右子树
        }
    }
    void preOrder(TreeNode *temp)
    {
        if (temp != NULL)
        {
            printf("node value: %d\n",temp->val);
            preOrder(temp->left);
            preOrder(temp->right);
        }
    }
};

int isSameTreeTest()
{
    Solution A;
    TreeNode *p = new TreeNode(10);
    TreeNode *q = new TreeNode(20);
    TreeNode *newTree1;
    TreeNode *newTree2;
    
    A.CreateBiTree(newTree1);
    A.CreateBiTree(newTree2);
    //向tree里面添加元素
    printf("tree1\n");
    A.preOrder(newTree1);
    printf("tree2\n");
    A.preOrder(newTree2);

    if(1 == A.isSameTree(p, q))
        printf("is same tree\n");
    else
        printf("is not same tree\n");
    return 0;
}
