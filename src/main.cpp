#include <iostream>
#include <string>
#include <cstring>
#include <stdio.h>
#include "global.h"
using namespace std;

FILE*filestream;
FILE*stream;

int main(int argc, char *argv[])
{
    short a = 0x12;
    short b = 0x34;
    short c = 0x56;
    short d = 0x78;
    long long f = 0;

    f = (f | ( (long long)d << 48));
    
    printf("d = 0x%lx\n",f);
    
    printf("sizeof a = %d\n",sizeof(a));
    f = 0;

    f = (f | (a));
    f = (f | (b << 16));
    f = (f | (c << 32));
    f = (f | (d << 48));
    printf("f = %lx\n",f);
  
    f = (f | ( (d << 48)| (c << 32) | (b << 16) | a));
    printf("d = %lx\n",d);
    if(0 != substringTest())
    {
        printf("error in substringTest\n");
    }
    if( 0 != findMedianSortedArraysTest())
    {
        printf("error in findMedianSortedArraysTest\n");
    }
    return 0;
}




