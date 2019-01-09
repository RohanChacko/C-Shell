#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "headers.h"

void echo(char command_str[][50], int pos)
{
        int echo_flag[2] ={0};

        char string[500];
        char env_string[500];
        env_string[0]='\0';
        int str_pos = 0;
        int env_pos = 0;
        int env_flag = 0;
        for(int j = 1; command_str[j][0]!='\0'; ++j)
        {
                for(int i = 0; command_str[j][i]!='\0'; ++i)
                {
                        if(command_str[j][i] == '$' || (command_str[j][0]=='~' && command_str[j][1]=='\0') || (command_str[j][0] == '~' && command_str[j][1] == '+' && command_str[j][2] =='\0'))
                        {
                                if(command_str[j][i] =='$')
                                {
                                        ++i;

                                        while(command_str[j][i]!='"' && command_str[j][i]!='\0' && command_str[j][i]!=' ')
                                        {
                                                env_string[env_pos++] = command_str[j][i++];
                                        }

                                        if(str_pos>0)
                                        {
                                                string[str_pos] = '\0';
                                                printf("%s", string);
                                                string[0] = '\0';
                                        }
                                        char *env_var;
                                        env_var = getenv(env_string);
                                        printf("%s", env_var);
                                }
                                else if(i == 0 && command_str[j][i] == '~' && command_str[j][i + 1] == '+')
                                {
                                        env_flag = 1;
                                        pwd();
                                }
                                else if(i == 0 && command_str[j][0]=='~')
                                {
                                        env_flag = 1;
                                        printf("%s",getenv("HOME"));
                                }


                        }

                        if(command_str[j][i]!= '"' && env_flag == 0)
                        {
                                string[str_pos] = command_str[j][i];
                                ++str_pos;
                        }
                }
                string[str_pos++] = ' ';
                env_flag = 0;
        }
        string[str_pos] = '\0';
        printf("%s",string);
        printf("\n");
}
