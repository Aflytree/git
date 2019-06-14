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
#include "global.h"

using namespace std;

/*  输入一字符串，输出为最大不相同字符的长度
 *　"abcabcbb"  abc 3
 *  "bbbbbbb"   b   1
 *  "pwwkew"    wke 3
 *
 */
int substringTest(){
    map<string, int> afMap;
    afMap["af"] = 100;
    afMap.insert(pair<string, int>("yh", 120));
    if(afMap.find("af") != afMap.end())
    {
        cout<<"get it"<<endl;
    }
    else
    {
        cout<<"do not get it"<<endl;
    }
    int nSize = afMap.size();
    cout<<"nSize = "<<nSize<<endl;

    return 0;
}

