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
#include <math.h>
#include "global.h"
using namespace std;

#define MAX_INT 2147483647
#define MIN_INT -2147483648

/*  string -> int
 *　"123" -> 123
 */
int myStrToInt(string s)
{
    int value = 0;
    int i = 0;
    int len = s.size();
    int negative_flag = 0;
    int invalid_data = 0;
    long ret_tmp = 0;

    for(i = 0;i < s.size();i++)
    {
        if(s[len - i - 1] == ' ')
        {
            invalid_data++;
            continue;
        }
        if(s[len - i - 1] == '-')
        {
            negative_flag = 1;
            continue;
        }

        if('a' <= s[len - i - 1]  &&  s[len - i -1] <= 'z')
        {
            invalid_data++;
            continue;
        }
        

        ret_tmp += pow(10, i - invalid_data) * (s[len - i - 1] -'0');   
        if(ret_tmp > MAX_INT)
        {
            return  MAX_INT;
        }

        printf("ret_tmp = %d\n",ret_tmp);

    }
         
    if(negative_flag == 1)
    {
        printf("ret_tmp1 = %d\n",ret_tmp);
        value = -value;
    }

    return value;
}

int myStrToIntTest()
{
    string ss =" wiht data 1236 - w";
    int val  = myStrToInt(ss);
    cout<<"val is  = "<<val<<endl;
    return 0;
}

