#include<stdarg.h>
#include<stdio.h>

//int vsprintf(char *str, const char *format, va_list arg) 使用参数列表发送格式化输出到字符串
int sum(int, ...);
char buffer[80];

int vspfunc(char *format, ...)
{
   va_list aptr;
   int ret;

   va_start(aptr, format);
   ret = vsprintf(buffer, format, aptr);
   va_end(aptr);

   return(ret);
}

int testSum(void)
{
   int i = 5;
   float f = 27.0;
   char str[50] = "runoob.com";
   printf("10、20 和 30 的和 = %d\n",  sum(3, 10, 20, 30) );
   printf("4、20、25 和 30 的和 = %d\n",  sum(4, 4, 20, 25, 30) );

   vspfunc("%d %f %s", i, f, str);
   printf("%s\n", buffer);

   return 0;
}

int sum(int num_args, ...)
{
   int val = 0;
   va_list ap;
   int i;

   va_start(ap, num_args);
   for(i = 0; i < num_args; i++)
   {
      val += va_arg(ap, int);
   }
   va_end(ap);
 
   return val;
}
