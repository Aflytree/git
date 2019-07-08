#include <stdio.h>
#include "global.h"
#include <cstdlib>

#define TEST(x, y, z, ret) printf("%s\r\n", ret==A.binarySearch(x, y, z)? "Successed":"Failed")

#define MAX_INT 2147483647
#define MIN_INT -2147483648

class Solution {
public:
    int binarySearch(int* input, int input_size, int target) {
        int left = 0;
        int right = input_size - 1;
        while(left <= right)
        {
            int mid = (left + right) / 2;
            
            if(input[mid] == target) return mid;
            if(input[mid] > target)
            {
                right = mid - 1;
            }
            else
            {
                left = mid + 1;
            }
        } 
        
        return -1;
    }
};

void binarySearchTest()
{
    Solution A;
    int ret;
    int nums[7]={-1, 0, 3, 4, 5, 9, 12};
 
    TEST(nums,7, 9, 5);

    if(-1 != (ret = A.binarySearch(nums, 7, 9)))
    {
        printf("binarySearh pos =%d\n", ret);
    }
    else
    {
        printf("error in binarySearh\n");
    }
}
