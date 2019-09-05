/*================================================================
*   Copyright (C) 2019 Lynxi Ltd. All rights reserved.
*   
*   @file       ：vectorTest.cpp
*   @author     ：afly
*   @date       ：2019.08.16
*   @description：
*
*================================================================*/



/**
*@param a     :
*@param b     :
*@retval      :
*@brief       :
*/

#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <stack>
#include <queue>
#include "global.h"

using namespace std;

class stackWithMax
{
    private:
        stack<int> valueStack;
        stack<int> maxStack;
    public:
        void push(int);
        int pop();
        int max();      
};

void
stackWithMax::push(int value)
{
    if(maxStack.empty())
    {
        maxStack.push(value);
    }
    if(maxStack.top() <= value)
    {
        maxStack.push(value);
    }
#if 0
    if(maxStack.empty() || maxStack.top <= value)
    {
        maxStack.push(value);
    }
#endif
    valueStack.push(value);
}
int 
stackWithMax::pop()
{
    int value = valueStack.top();
    valueStack.pop();

    if(value == maxStack.top())
    {
        maxStack.pop();
    }
    return value;
}
int 
stackWithMax::max()
{
    return maxStack.top();
}

int stackWithMaxTest()
{
    stack <int> myStack;
    printf("myStack.top = %d\n", myStack.top());

    return 0;
}

