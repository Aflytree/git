/*================================================================
*   Copyright (C) 2019 Lynxi Ltd. All rights reserved.
*   
*   @file       ：vectorTest.cpp
*   @author     ：afly
*   @date       ：2019.06.07
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

//queue used for: BFS breadth first search广度优先算法
//queue:buffer 生产者和消费者模型 
using namespace std;

int stackAndQueueTest()
{
    stack <int> myStack;
    myStack.push(10);
    myStack.push(20);

    printf("myStack.top = %d\n", myStack.top());

    queue<int> myQueue;
    myQueue.push(11);
    myQueue.push(22);
    printf("myQueue.front = %d\n", myQueue.front());

    myQueue.pop();

    printf("myQueue.front = %d\n", myQueue.front());

    return 0;
}

