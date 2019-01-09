#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>

volatile sig_atomic_t listener;

void event_listener(int signum)
{
        listener = 1;
}

int clocker(char command_str[][50])
{

        listener = 0;

        signal(SIGINT, event_listener);

        while(!listener)
        {
                time_t raw_time;
                struct tm* time_;
                time(&raw_time);
                time_ = localtime(&raw_time);

                char time_dis[101];

                strftime(time_dis, 100, "%d %b %y, %H:%M:%S\n", time_);
                printf("%s", time_dis);
                sleep(atoi(command_str[2]));
        }

        return 1;

}
