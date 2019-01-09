#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void pinfo(char command[][50], int pos, char home_dir[])
{
        int arg_flag = 0;
        int t = 0;
        long size = 0;
        char buffer[505];
        char exec_val[505];
        char *temp;

        pid_t pid;
        //Try to check if more than one argument is given. If so, error.
        if(command[1][0]!='\0')
        {
                for(int i = 0; command[1][i]!='\0'; ++i)
                {
                        if(!(command[1][i] >= '0' && command[1][i] <= '9'))
                        {
                                arg_flag = -1;
                                printf("pinfo: Please Enter a number for PID\n");
                                return;
                        }
                }

                if(arg_flag == 0)
                {
                        arg_flag = 1;
                        pid = atoi(command[1]);
                }
        }
        else
        {
                pid = getpid();
        }

        sprintf(buffer,"/proc/%d/status",pid);
        FILE *fd = fopen(buffer,"r");
        if(fd == NULL)
        {
                printf("Error: while retrieving information about required process with pid %d\n", pid );
                return;
        }
        printf("pid -- %d\n",pid);                      //PID value

        for(int i = 0; i<3; ++i)                        //Status present on 3rd line
                getline(&temp,&size,fd);
        printf("%s",temp);

        for(int i=0; i<15; i++)                         //Memory allocation
                getline(&temp,&size,fd);
        printf("%s",temp);

        fclose(fd);

        sprintf(buffer,"/proc/%d/exe",pid);

        t  = readlink(buffer,exec_val,500);

        if(t == -1)
                printf("Error getting executable path for process with pid: %d\n", pid);
        else
        {
                exec_val[t] = '\0';
                char exec_display[505];

                if(strcmp(exec_val,home_dir)<0)
                {
                        strcpy(exec_display, exec_val);
                }
                else
                {
                        int k = 1;
                        exec_display[0] = '~';

                        for(int i = strlen(home_dir); i<strlen(exec_val); ++i)
                        {
                                exec_display[k++] = exec_val[i];
                        }
                        exec_display[k] = '\0';
                }

                printf("Executable path -- %s\n",exec_display);
        }
}
