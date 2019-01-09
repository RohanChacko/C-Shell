#include <stdio.h>
#include <unistd.h>
  
int pwd()
{
        char cwd[500];
        getcwd(cwd, 500);
        printf("%s\n",cwd );
        return 1;
}
