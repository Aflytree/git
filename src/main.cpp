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
    if(0 != substringTest())
    {
        printf("error in substringTest\n");
    }
    return 0;
}




