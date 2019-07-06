#include <stdio.h>
#include "global.h"

#define TEST(x, y) printf("%12d --->> %-12d  %s\r\n", x, A.reverse(x), y==A.reverse(x)? "Successed":"Failed")

#define MAX_INT 2147483647
#define MIN_INT -2147483648

class Solution {
public:
    int reverse(int x) {
        int tmp = 0;
        int i = 0;
        int y = 0;
        int negative_flag = 0;
        int change = 0;
        
        if(i < 0)
        {
            negative_flag = 1;
            tmp = -tmp;
        }
    
        tmp = x % 10;
        y =  x / 10;
        
        if(tmp ==0 && y == 0)
            return 0;

        while(tmp == 0)
        {
            tmp  = y % 10;
            y = y  / 10;
        }

        while(y != 0 || tmp != 0)
        {
        //214748364 0 + 1~9
        //214748364 1 + 1~9
        //214748364 2 + 1~9
        //214748364 3 + 1~9
        //214748364 4 + 1~9
        //214748364 5 + 1~9
        //214748364 6 + 1~9
        //214748364 7 + 1~9
        //214748364 8 + 1~9
        //214748364 9 + 1~9
            if(change  > MAX_INT / 10  || change < MIN_INT /10)
            {
                return 0;
            }
            change = change * 10  + tmp;
        
            tmp = y % 10;
            y = y / 10;
        }
   
        if(negative_flag == 1)
        {
            change = -change;
        }
        
        return change;
    }
};

int reverseTest()
{
    Solution A;
    A.reverse(123);
    TEST(123, 321);
    TEST(120, 21);
    TEST(0, 0);
    TEST(90100, 109);
    return 0;
}
