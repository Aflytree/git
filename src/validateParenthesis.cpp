/*================================================================
*   Copyright (C) 2019 Lynxi Ltd. All rights reserved.
*   
*   @file       validateParenthesis.cpp
*   @author     ：afly
*   @date       ：2019.08.18
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

bool isLeftParentheses(char input)
{
    return input == '(' || input == '[' || input =='{';
}
bool isMatchParentheses(char left, char right)
{
    switch (left)
    {
    case '(':
        return right ==')';
    case '[':
        return right == ']';
    case '{':
        return right = '}';
    default:
        break;
    }
    return false;
}

bool validateParenthesis(string input)
{
    stack <char> parenthesesStack;
    for(int i = 0;i < input.length();i++)
    {
        if(isLeftParentheses(input[i]))
        {
            parenthesesStack.push(input[i]);
        }
        else
        {
            if(parenthesesStack.empty() || !isMatchParentheses(parenthesesStack.top(), input[i]))
                return false;
            parenthesesStack.pop();
        }
    }
    return parenthesesStack.empty();
}

int isValidParenthesisTest()
{
    stack <int> myStack;
    string ss1 = "()";
    string ss2 = "(]";



    printf("myStack.top = %d\n", myStack.top());

    return 0;
}

