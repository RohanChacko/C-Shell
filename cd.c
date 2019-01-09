#include <stdio.h>
#include <unistd.h>
#include <string.h>

void cd(char command_str[][50], int pos, char home_dir[])
{
        int error_flag;
        if(command_str[1][0] == '\0' || strcmp(command_str[1], "~") == 0 || strcmp(command_str[1], "~/") == 0)
        {
                error_flag = chdir(home_dir);

                if(error_flag == -1)
                {
                        printf("bash: cd: %s: No such file or directory\n", command_str[1]);
                        return;
                }

                return;
        }

        char change_dir[505];
        getcwd(change_dir, 500);

        if(strcmp(command_str[1], "..")==0 && change_dir[0]=='/' && change_dir[1]!='\0')
        {
                int cur_path_len = strlen(change_dir);

                for(int i = cur_path_len - 1; i>=0; --i)
                {
                        if(change_dir[i]!='/')
                        {
                                change_dir[i] = '\0';
                        }
                        else
                        {
                                if(change_dir[1]!='\0')
                                        change_dir[i] = '\0';
                                break;
                        }
                }

                error_flag = chdir(change_dir);

                if(error_flag == -1)
                {
                        printf("bash: cd: %s: No such file or directory\n", command_str[1]);
                }

        }

        else if(command_str[1][0] == '/')                                       //Absolute path defined as starting from root dir
        {

                error_flag = chdir(command_str[1]);

                if(error_flag == -1)
                {
                        printf("bash: cd: %s: No such file or directory\n", command_str[1]);
                }
        }

        else if(strcmp(command_str[1], "~/")>0)
        {
                char temp_dir[505];
                strcpy(temp_dir, home_dir);
                strcat(temp_dir, "/");
                getcwd(change_dir, 500);

                int k = 2;
                for(int i = strlen(change_dir); command_str[1][k]!='\0'; )
                {
                        temp_dir[i++] = command_str[1][k++];
                }
                error_flag = chdir(temp_dir);

                if(error_flag == -1)
                {
                        printf("bash: cd: %s: No such file or directory\n", command_str[1]);
                }

        }

        else if(strcmp(command_str[1],".")!=0 && strcmp(change_dir, "/")!=0)
        {
                char temp_dir[505];
                strcpy(temp_dir, change_dir);
                strcat(temp_dir, "/");
                strcat(temp_dir, command_str[1]);

                error_flag = chdir(temp_dir);

                if(error_flag == -1)
                {
                        printf("bash: cd: %s: No such file or directory\n", command_str[1]);
                }

        }


}
