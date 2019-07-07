/*================================================================
*   Copyright (C) 2019 Lynxi Ltd. All rights reserved.
*   
*   @file       ：vectorTest.h
*   @author     ：afly
*   @date       ：2019.06.07
*   @description：
*
*================================================================*/
#ifndef _VECTORTEST_H
#define _VECTORTEST_H
#endif //VECTORTEST_H


struct TreeNode {
    char val;
    TreeNode *left;
    TreeNode *right;
    TreeNode(int x) : val(x), left(NULL), right(NULL) {}
};

/**
*@param a     :
*@param b     :
*@retval      :
*@brief       :
*/
int substringTest();
int findMedianSortedArraysTest();
int myStrToIntTest();
int reverseTest();
int isSameTreeTest();
