#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <readline/readline.h>                                               //For autocomplete and command history sudo apt install libreadline-dev
#include <readline/history.h>
#include "headers.h"

typedef struct
{
        int id;
        char proc_name[100];
        int remind_flag;
        int state;
} Bg_proc;

int proc_count = 0;
int reminder_count;
Bg_proc proc_arr[100];
char display_path_name[505];
int cur_pid = -1;
char cur_proc_name[101];
int main_pid;
int kill_id[100];
int kill_count = 0;

//SIGINT handler
void CTRL_C(int sig)
{
        if(getpid() != main_pid)
                return;

        if(cur_pid != -1)
        {
                kill(cur_pid, SIGINT);
        }
        signal(SIGINT, CTRL_C);
}

//SIGTSTP handler
void CTRL_Z(int sig)
{
        if(getpid() != main_pid)
                return;

        if(cur_pid != -1)
        {
                //kill((pid_t)cur_pid, SIGTTIN);
                kill((pid_t)cur_pid, SIGTSTP);

                proc_arr[++proc_count].id = cur_pid;
                strcpy(proc_arr[proc_count].proc_name, cur_proc_name);
                proc_arr[proc_count].state = 0;
                //printf("--%d--%s--\n",cur_pid, cur_proc_name );
                if(strcmp(proc_arr[proc_count].proc_name, "remindme") == 0)
                        proc_arr[proc_count].remind_flag = 1;

        }
        signal(SIGTSTP, CTRL_Z);
}

//SIGCHLD handler - Asynchronous background process termination message
void chld_handler(int sig)
{
        if(getpid()!=main_pid)
                return;

        for(int i=1; i<=proc_count; i++)
        {
                int status;
                pid_t return_pid = waitpid((pid_t)proc_arr[i].id, &status, WNOHANG);

                if(proc_arr[i].id == return_pid )
                {
                        if(proc_arr[i].remind_flag == 1)

                                printf("\nReminder: %s\n", proc_arr[i].proc_name);
                        else
                        {
                          if (WIFEXITED(status))
                              fprintf(stderr,"\n%s with pid %d exited with exit status %d.\n",proc_arr[i].proc_name,proc_arr[i].id, WEXITSTATUS(status));

                          if (WIFSIGNALED(status))
                              fprintf(stderr,"\n%s with pid %d exited via signal %d.\n",proc_arr[i].proc_name,proc_arr[i].id, WTERMSIG(status));
                        }
                }
                else
                {
                        kill_id[kill_count++] = i;
                }
        }

        remove_proc(kill_id, kill_count);
        kill_count = 0;


        signal(SIGCHLD, chld_handler);
}

int forever()
{
        char *username;
        char sys_name[500];
        char home_dir[500];
        getcwd(home_dir,500);
        int home_path_len = strlen(home_dir);
        int cd_flag = 0;
        int cur_dir_len = 0;
        rl_bind_key('\t', rl_complete);       //Bind tab press to function as autocomplete

        signal(SIGTSTP, CTRL_Z);
        signal(SIGINT, CTRL_C);
        signal(SIGCHLD, chld_handler);

        while(1)
        {
                cur_pid = -1;
                username=getenv("USER");
                gethostname(sys_name,500);
                int stat;

                change_display_path(home_dir);
                char display_prompt[505];
                sprintf(display_prompt,"<%s%s@%s%s:%s%s%s>","\x1B[1;32m",username,sys_name,"\x1B[0m","\x1B[1;34m",display_path_name,"\x1B[0m");

                char *buffer;
                size_t len = 0;
                int flag = 0;
                size_t buffer_size = 0;

                buffer = readline(display_prompt);
                // getdelim(&buffer, &buffer_size, '\n', stdin);

                if (flag==-1)
                {
                        printf("Error: No command read.\n");
                        exit(1);
                }

                //Tokenize the command stream. Max 500 commands in one line

                char* token = strtok(buffer, ";");
                char sep_commands[500][500];
                int counter = 0;
                while (token != NULL)
                {
                        strcpy(sep_commands[counter], token);
                        sep_commands[counter++][strlen(token)] = '\0';
                        token = strtok(NULL, ";");
                }
                sep_commands[counter][0] = '\0';

                for(int i=0; sep_commands[i][0]!='\0'; ++i)
                {
                        command_extract(sep_commands[i], home_dir);
                }
                add_history(buffer);
                free(buffer);

        }
}

int main()
{
        main_pid = getpid();
        forever();
        return 0;
}
