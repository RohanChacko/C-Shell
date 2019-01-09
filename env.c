#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

void setenvar(char command_str[][50])
{
  if(command_str[1][0]=='\0')
  {
    printf("Error: Command requires atleast one argument");
    return;
  }

  else if(command_str[3][0]!='\0')
  {
    printf("Error: Command requires maximum of two arguments");
    return;
  }

  if(command_str[2][0] == '\0')
  {
    setenv(command_str[1], " " , 1);
  }
  else
  {
    setenv(command_str[1],command_str[2] , 1);
    printf("%s",getenv(command_str[1]));
  }

  return;
}

void unsetenvar(char command_str[][50])
{

  if(command_str[1][0] == '\0')
  {
    printf("Error: Command requires environment variable");
    return;
  }

  else
  {
    unsetenv(command_str[1]);
  }

}
