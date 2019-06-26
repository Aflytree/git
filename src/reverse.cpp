class Solution {
public:
    int reverse(int x) {
        int tmp = 0;
        int i = 0;
        int y = 0;
        int negative_flag = 0;
        int change = 0;
        if(i > 32767 || i < -32768)
            return 0;
        if(i < 0)
        {
            negative_flag = 1;
            tmp = -tmp;
        }
            
    
        tmp = x % 10;
        y =  x / 10;
        while(tmp != 0)
        {
            if(change  > 846384741)
            {
                return 0;
            }
            change = change * 10  + tmp;
        
            tmp = y % 10;
            y = y / 10;
        }
        if(negative_flag == 1)
        {
            if( -change < -32768)
                return 0;
            change = -change;
        }
        
        return change;
    }
};
