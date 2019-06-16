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
 *　"abc abc bb"  abc 3
 *  "bbbbbbb"   b   1
 *  "pwwkew"    wke 3
 *
 */
int findLongestDiffChar(string s)
{
    map<char, int>m;
    map <char, int>::iterator iter;
    int lastSamePos = -1;
    int i = 0, maxLen = 0;
    cout<<"s.size() = "<<s.size()<<endl;
    for(i=0;i<s.size();i++)
    {
        //s[i]已经在map m中
        if(m.find(s[i]) != m.end())
        {
            if(m[s[i]] > lastSamePos )
            {
                lastSamePos = m[s[i]];
            }
            else
            {
                if(i - lastSamePos > maxLen)
                {   
                   maxlen = i - lastSamePos;
                }
            }
            
            cout<<"i = "<<i<<" lastSamePos  if= "<<lastSamePos<<endl;       
        }
        else
        {
            cout<<"i = "<<i<<" lastSamePos else = "<<lastSamePos<<endl; 
            if(i - lastSamePos > maxLen)
            {
                maxLen = i - lastSamePos;
            }
          
            cout<<"maxLen is  = "<<maxLen<<endl;
        }     
   

        m[s[i]] = i;
        for(iter = m.begin();iter != m.end();iter++)
        {
            cout<<"iter = "<<iter->first<<" "<<iter->second<<endl;
        }
    }
    cout<<"maxLen = "<<maxLen<<endl; 
    return maxLen;
}
int substringTest()
{
    string ss ="tmmzuxt";
    //string ss = "abcabcmmabcdefghi";
    //string ss ="nfpdmpi";


    int len  = findLongestDiffChar(ss);
    cout<<"maxLen is  = "<<len<<endl;

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

