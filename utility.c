#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "headers.h"

typedef struct
{
        int id;
        char proc_name[100];
        int remind_flag;
        int state;
} Bg_proc;

extern int proc_count;
extern int reminder_count;
extern int cur_pid;
extern Bg_proc proc_arr[100];
extern char display_path_name[505];
extern char cur_proc_name[101];

char exec_command[100][50];
int in_file = 0;
int out_file = 0;
int in_fd = 0;
int out_fd = 1;

int execute(char command[][50], char home_dir[], int bg_proc, int reminder, int pipe_flag);
int piedpiper(char command_str[], char home_dir[], int bg_proc, int reminder);
void redirector(char command[][50]);

void change_display_path(char home_dir[])
{
        char present_dir[505];
        getcwd(present_dir, 500);
        if(strcmp(present_dir,home_dir)<0)
        {
                strcpy(display_path_name, present_dir);
        }
        else
        {
                int k = 1;
                display_path_name[0] = '~';

                for(int i = strlen(home_dir); i<strlen(present_dir); ++i)
                {
                        display_path_name[k++] = present_dir[i];
                }
                display_path_name[k] = '\0';
        }
}

void command_extract(char command_str[], char home_dir[])
{
        char command[50][50];

        int j = 0, i = 0, status;

        char org_str[500];
        strcpy(org_str, command_str);

        char* token = strtok(command_str, " \t\n");

        while (token != NULL)
        {
                strcpy(command[i], token);
                command[i++][strlen(token)] = '\0';
                token = strtok(NULL, " \t\n");
        }

        command[i][0] = '\0';

        int bg_proc = 0;
        int pipe_flag = 0;
        int reminder = 0;

        for(i = 0; command[i][0]!='\0'; ++i)
        {
                //printf("%s\n",command[i] );
                for(j = 0; command[i][j]!='\0'; ++j)
                {
                        if(command[i][j] == '&' && (command[i][j+1] ==' ' || command[i][j+1]=='\0') )
                        {
                                bg_proc = 1;
                                break;
                        }

                        else if(command[i][j] == '|')
                        {
                                pipe_flag = 1;
                        }
                }

                if(bg_proc == 1)
                        break;
        }

        if(pipe_flag == 1)
                piedpiper(org_str, home_dir, bg_proc, reminder);
        else
                execute(command, home_dir, bg_proc, reminder, pipe_flag);
        return;
}

//Piping logic
int piedpiper(char command_str[], char home_dir[], int bg_proc, int reminder)
{
        char command[50][50];

        int j = 0, i = 0, status,input_fd = 0;

        char* token = strtok(command_str, "|");

        while (token != NULL)
        {
                strcpy(command[i], token);
                command[i++][strlen(token)] = '\0';
                token = strtok(NULL, "|");
        }

        command[i][0] = '\0';

        for (i = 0; command[i][0]!='\0'; ++i)
        {
                int pipe_fd[2];
                pipe(pipe_fd);
                //printf("extracted: %s\n",command[i] );
                pid_t piped_pid = fork();

                if(piped_pid==0)
                {
                        dup2(input_fd, 0);

                        if(command[i+1][0]!='\0')
                                dup2(pipe_fd[1], 1);

                        close(pipe_fd[0]);

                        char command_pipe[50][50];

                        int j = 0;

                        char* tokener = strtok(command[i], " \t\n");

                        while (tokener != NULL)
                        {
                                strcpy(command_pipe[j], tokener);
                                command_pipe[j++][strlen(tokener)] = '\0';
                                tokener = strtok(NULL, " \t\n");
                        }

                        command_pipe[j][0] = '\0';
                        //printf("%s: pop\n",command_pipe[0] );
                        execute(command_pipe,home_dir, bg_proc, reminder, 1);
                        //printf("done\n" );
                        exit(2);
                }
                else
                {
                        //printf("::%d\n",i );
                        wait(NULL);
                        close(pipe_fd[1]);
                        input_fd = pipe_fd[0];
                }

        }

}

//Redirection logic
void redirector(char command[][50])
{
        int pos = 0;
        int i, j;

        //FORMAT: COMMAND 1 [SPACE] >/</>>/<< [SPACE] COMMAND 2
        for (i = 0; command[i][0] != '\0'; ++i)
        {

                //printf("%d\n",i);
                if(strcmp(command[i], "<") == 0)
                {
                        in_file = 1;
                        if(command[i+1][0]!='\0')
                        {
                                in_fd = open(command[++i], O_RDONLY);
                                if(in_fd == -1)
                                {
                                        fprintf(stderr, "Error: Enter valid input file\n");
                                        return;
                                }
                        }
                        else
                        {
                                fprintf(stderr,"Error:Enter valid input file\n");
                                return;
                        }

                }
                else if(strcmp(command[i], ">") == 0)
                {
                        out_file = 1;
                        if(command[i+1][0]!='\0')
                        {
                                out_fd = open(command[++i], O_WRONLY | O_TRUNC | O_CREAT, 0644);
                        }
                        else
                        {
                                fprintf(stderr,"Error: Enter valid output file\n");
                                return;
                        }
                }
                else if(strcmp(command[i], ">>") == 0)
                {
                        out_file = 1;
                        if(command[i+1][0]!='\0')
                                out_fd = open(command[++i], O_WRONLY | O_APPEND | O_CREAT, 0644);
                        else
                        {
                                fprintf(stderr,"Error: Enter valid output file\n");
                                return;
                        }
                }
                else
                {
                        strcpy(exec_command[pos++], command[i]);
                }
        }
        exec_command[pos][0] = '\0';

        return;
}

//Execute function to execute commands( Builtin and otherwise)
int execute(char command[][50], char home_dir[], int bg_proc, int reminder, int pipe_flag)
{

        if(strcmp(command[0], "remindme") == 0)
        {
                bg_proc = 1;
                reminder = 1;                
        }

        if(strcmp(command[0], "quit") == 0 )
        {
                printf("See you soon! Bye.\n");
                exit(0);
        }

        else if(strcmp(command[0], "cd") == 0)
        {
                cd(command,0, home_dir);
        }
        else if(strcmp(command[0], "clock") == 0)
        {
                clocker(command);
        }
        else if(strcmp(command[0], "fg") == 0)
        {
                fg(command);
        }
        else if(strcmp(command[0], "bg") == 0)
        {
                bg(command);
        }
        else
        {
                pid_t pid, wpid;

                pid = fork();
                cur_pid = pid;
                strcpy(cur_proc_name, command[0]);
                //printf("%d--%s\n",cur_pid, cur_proc_name );

                if (pid == 0)
                {

                        if(bg_proc == 1)
                        {
                                if(setpgid(0,0)!= 0)
                                {
                                        fprintf(stderr, "Unable to start process in background\n");
                                        exit(1);
                                }
                        }

                        redirector(command);
                        if(in_file)
                        {
                                dup2(in_fd,0);
                                close(in_fd);
                        }

                        if(out_file)
                        {
                                dup2(out_fd,1);
                                close(out_fd);
                        }

                        if(strcmp(exec_command[0], "pwd")==0)
                        {
                                pwd();
                        }
                        else if(strcmp(exec_command[0], "jobs") == 0)
                        {
                                jobs();
                        }
                        else if (strcmp(exec_command[0], "kjob") == 0)
                        {
                                kjobs(exec_command);
                        }
                        else if(strcmp(exec_command[0], "overkill") == 0)
                        {
                                overkill();
                        }
                        else if(strcmp(exec_command[0], "echo") == 0)
                        {
                                echo(exec_command, 1);
                        }
                        else if(strcmp(exec_command[0], "ls") == 0 )
                        {
                                ls(exec_command, 0);
                        }
                        else if(strcmp(exec_command[0], "pinfo") == 0)
                        {
                                pinfo(exec_command,0, home_dir);
                        }
                        else if(strcmp(exec_command[0],"setenv") == 0)
                        {
                                setenvar(exec_command);
                        }
                        else if(strcmp(exec_command[0], "unsetenv") == 0)
                        {
                                unsetenvar(exec_command);
                        }
                        else
                        {
                                int i = 0;
                                char *args[50];

                                if(reminder == 1)
                                {

                                        args[0] = (char *)malloc(strlen("sleep")+1);
                                        strcpy(args[0], "sleep");

                                        args[1] = (char *)malloc(505);
                                        strcpy(args[1], exec_command[1]);
                                        args[2] = NULL;
                                }
                                else{
                                        for(i = 0; exec_command[i][0]!='\0' && strcmp(exec_command[i],"&")!=0; ++i)
                                        {
                                                args[i] = (char *)malloc(strlen(exec_command[i])+1);
                                                strcpy(args[i], exec_command[i]);
                                                //printf("seq: %s\n",exec_command[i] );
                                        }
                                        args[i] = NULL;
                                }
                                if(execvp(args[0], args) == -1)
                                {
                                        printf("%s: command not found",exec_command[0]);
                                }
                                printf("\n");
                        }
                        exit(0);
                }
                else if (pid > 0)
                {
                        if(bg_proc == 1)
                        {
                                kill(pid, SIGTTIN);
                                kill(pid, SIGCONT);
                                proc_count++;
                                proc_arr[proc_count].id = (int) pid;
                                strcpy(proc_arr[proc_count].proc_name,command[0]);
                                proc_arr[proc_count].remind_flag = 0;
                                proc_arr[proc_count].state = 1;
                                //printf("name: %s\n", proc_arr[proc_count].proc_name);

                                if(reminder == 1)
                                {
                                        int k = 0;
                                        proc_arr[proc_count].remind_flag = 1;

                                        for(int i = 2; command[i][0]!='\0'; ++i)
                                        {
                                                for(int j = 0; command[i][j]!='\0'; ++j)
                                                {
                                                        if(command[i][j]!='"')
                                                                proc_arr[proc_count].proc_name[k++] = command[i][j];
                                                }
                                                proc_arr[proc_count].proc_name[k++] = ' ';
                                        }

                                        proc_arr[proc_count].proc_name[k] = '\0';
                                }
                        }
                        else if(bg_proc == 0)
                        {
                          //printf("inactive\n" );
                          waitpid(pid,NULL,WUNTRACED);
                        }
                }
                else
                {
                        fprintf(stderr, "Error: Cannot fork a child process\n");
                        exit(1);
                }
        }
        return 1;
}

//Remove background processes
void remove_proc(int kill_id[], int kill_count)
{
        Bg_proc temp_proc[100];
        int temp_proc_count=0;

        for(int i = 0; i<kill_count; ++i)
        {
                temp_proc[i] = proc_arr[kill_id[i]];
        }

        for(int i = 0; i<kill_count; ++i)
        {
                proc_arr[i+1] = temp_proc[i];
        }

        proc_count = kill_count;
}
