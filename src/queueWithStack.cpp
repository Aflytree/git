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
*@brief       : 两个stack实现一个queue
*/

#include <iostream>
#include <vector>
#include <array>
#include <map>
#include <stack>
#include <queue>
#include "global.h"

using namespace std;

class queueWithStack
{
    private:
        stack<int> inputStack;
        stack<int> outputStack;
    public:
        void enqueue(int value);
        int dequeue();      
};

void
queueWithStack::enqueue(int value)
{
    inputStack.push(value);
}

int
queueWithStack::dequeue()
{
    int value;
    if(!outputStack.empty())
    {
        value = outputStack.top();
        outputStack.pop();
        return value;
    }

    while(!inputStack.empty())
    {
        outputStack.push(inputStack.top());
        inputStack.pop();
    }
    value = outputStack.top();
    outputStack.pop();
    return value;
}

int queueWithStackTest()
{
    stack <int> myStack;
    printf("myStack.top = %d\n", myStack.top());

    return 0;
}

