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

/*  输入两个排好序的数组，输出他们的中位数
 *
 */
int findMedianSortedArrays(int m[], int n[])
//int findMedianSortedArrays(vector<int>m, vector<int>n)
{
    
    return 0;
}

int findMedianSortedArraysVector(vector<int>&m, vector<int>&n)
{
    int size_m = m.size();
    int size_n = n.size();
    int medium_m = 0;
    int medium_n = 0;
    cout<<"size_m = "<<size_m<<endl;
    cout<<"size_n = "<<size_n<<endl;
    if(size_m % 2 == 1)
    {
        medium_m = m[(size_m + 1) / 2];   
    }
    else
    {
        medium_m = (m[size_m / 2 - 1] + m[size_m / 2])/2 ;
    }
    cout<<"medium_m = "<<medium_m<<endl;
    

    return 0;
}
int findMedianSortedArraysTest()
{
    int a[] = {0,1,2,2,6,8};
    int b[] = {5,6,4,4,9,2};
    vector<int>k = {0,1,2,2,6,8};
    vector<int>j = {5,6,7,8,9}; 
    //findMedianSortedArrays(a, b); 
    findMedianSortedArraysVector(k, j);
    return 0;
}

