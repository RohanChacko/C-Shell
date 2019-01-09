#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
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
extern char cur_proc_name[101];

void jobs()
{
        for(int i=1; i<=proc_count; ++i)
        {
                if(proc_arr[i].remind_flag == 0)
                        printf("[%d] \t %s \t %s [%d]\n", i, proc_arr[i].state==1? "Running":"Stopped", proc_arr[i].proc_name, proc_arr[i].id);
                else
                        printf("[%d] \t %s \t remindme [%d]\n", i, proc_arr[i].state==1? "Running":"Stopped", proc_arr[i].id);
        }
}

void kjobs(char command_str[][50])
{

        if(command_str[3][0] != '\0')
        {
                printf("Error: Two arguments required\n");
                return;
        }

        int job_number = atoi(command_str[1]);
        int signal_number = atoi(command_str[2]);

        if(proc_count < job_number)
        {
          printf("Error: No such process exists\n");
          return;
        }
        kill(proc_arr[job_number].id, signal_number);

}

void bg(char command_str[][50])
{
    if(command_str[2][0] != '\0')
    {
      printf("Error: One argument required\n");
      return;
    }

    int pid = atoi(command_str[1]);

    if(proc_count < pid)
    {
      printf("Error: No such process exists\n");
      return;
    }

    kill(proc_arr[pid].id, SIGTTIN);                                            //To ensure no terminal input is not taken for a bg process
    kill(proc_arr[pid].id, SIGCONT);

    proc_arr[pid].state = 1;
}

void fg(char command_str[][50])
{
  if(command_str[2][0] != '\0')
  {
    printf("Error: One argument required\n");
    return;
  }

  int pid = atoi(command_str[1]);

  if(proc_count < pid)
  {
    printf("Error: No such process exists\n");
    return;
  }

  cur_pid = proc_arr[pid].id;
  strcpy(cur_proc_name, proc_arr[pid].proc_name);

  kill(proc_arr[pid].id, SIGCONT);

  waitpid(-1, NULL, WUNTRACED);

  if(proc_arr[pid].remind_flag == 1)
    printf("Reminder: %s\n", proc_arr[pid].proc_name);

  int kill_id[101];
  int kill_count = 0;

  for(int i = 1; i<=proc_count; ++i)
  {
    if(i != pid)
    {
      kill_id[kill_count++] = i;
    }
  }

  remove_proc(kill_id, kill_count);

  proc_count = kill_count;
  kill_count = 0;
}

void overkill()
{
        for(int i=1; i<=proc_count; ++i)
        {
                kill(proc_arr[i].id,SIGKILL);
        }

}
