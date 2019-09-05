#include <stdio.h>

void PROGRAM_EXIT(int reason, const char *fmt, ...)
{
    char tmp_buf[1024] ={0};
    va_list args;
    va_start(args, fmt);
    vsprintf(tmp_buf, fmt, args);

    printf("ERROR:%s",tmp_buf);

    switch (reason)
    {
    case 1:
        printf("please check file name");
        break;
    default:
        break;
    }

    printf("Program exit!\n");
    //exit(1);
}