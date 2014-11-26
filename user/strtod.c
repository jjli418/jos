#include <stdio.h>
#include <stdlib.h>
 
int main(void) 
{
    char *str = " 111.11 22.22 0X1.BC70A3D70A3D7P+6  1.18973e+4932";
    char *str_end;
 
    printf("%d %d\n", sizeof(long), sizeof(long long));
    printf("%f\n", strtod(str,&str_end));
    printf("str_end --> [%c]\n", *str_end);
    printf("%f\n", strtod(str_end,&str_end));
    printf("%g\n", strtod(str_end,&str_end));
    printf("%f\n", strtod(str_end, NULL));
 
    return 0;
}
