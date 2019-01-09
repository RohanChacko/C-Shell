#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int hide(const struct dirent *name)
{
        if(name->d_name[0] == '.') return 0;
        else return 1;
}

int hiddensort(const struct dirent **name1, const struct dirent **name2)
{
      const char *a = (*name1)->d_name;
      const char *b = (*name2)->d_name;

      if(a[0] == '.')
        a++;
      if(b[0] == '.')
        b++;

      return(strcmp(a,b));
}

int ls_l(char pathname[], char *filename)
{
        struct stat st;
        char timing[100];

        int t = lstat(pathname,&st);
        if(t < 0)
        {
                printf("Error\n");
                return 0;
        }

        struct passwd * uname = getpwuid(st.st_uid);
        struct group * ugroup = getgrgid(st.st_gid);
        struct tm * time = localtime(&st.st_mtime);

        /*--------------------------------------------------------------------------*/

        if(S_ISDIR(st.st_mode))
                printf("d");
        else if(S_ISLNK(st.st_mode))
                printf("l");
        else
                printf("-");

        if(st.st_mode & S_IRUSR)
                printf("r");
        else
                printf("-");

        if(st.st_mode & S_IWUSR)
                printf("w");
        else
                printf("-");

        if(st.st_mode & S_IXUSR)
                printf("x");
        else
                printf("-");

        if(st.st_mode & S_IRGRP)
                printf("r");
        else
                printf("-");

        if(st.st_mode & S_IWGRP)
                printf("r");
        else
                printf("-");

        if(st.st_mode & S_IWGRP)
                printf("w");
        else
                printf("-");

        if(st.st_mode & S_IXGRP)
                printf("x");
        else
                printf("-");

        if(st.st_mode & S_IROTH)
                printf("r");
        else
                printf("-");

        if(st.st_mode & S_IWOTH)
                printf("w");
        else
                printf("-");

        if(st.st_mode & S_IXOTH)
                printf("x");
        else
                printf("-");

        /*--------------------------------------------------------------------------*/

        printf(" %2lu", st.st_nlink); //Number of hardlinks
        printf(" %s", uname->pw_name);
        printf(" %s", ugroup->gr_name);
        printf(" %6ld ",st.st_size);

        strftime(timing,10,"%b",time);

        printf(" %s %2d", timing, time->tm_mday);
        printf(" %02d:%02d",time->tm_hour, time->tm_min);


        //Keeping different color
        if (S_ISDIR(st.st_mode))
                printf(" %s%s%s\n", "\x1B[1;34m", filename, "\x1B[0m");
        else if(S_ISLNK(st.st_mode))
        {
                char buffer[305];
                readlink(filename, buffer, 300);                                //Things we do for bonus marks
                printf(" %s%s%s -> %s\n", "\x1B[1;36m", filename, "\x1B[0m", buffer);
        }
        else if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
                printf(" %s%s%s\n", "\x1B[1;32m", filename, "\x1B[0m");
        else
                printf(" %s\n", filename);

        return st.st_blocks;

}

void ls(char command_str[][50], int pos)
{
        int ls_blk_size = 0;
        int ls_flag[2] = {0};
        int directory[505];
        int dir_count = 0;
        int no_path = 0;
        int i = 1;
        int items = 0;
        while(command_str[i][0]!='\0')
        {
                if(command_str[i][0] == '-')
                {
                        if(command_str[i][1] == 'a'|| command_str[i][2] == 'a')
                                ls_flag[0] = 1;

                        if(command_str[i][1] == 'l' || command_str[i][2] == 'l')
                                ls_flag[1] = 1;

                        if(command_str[i][1] == '\0')
                        {
                                printf("Error. No such command\n");
                                return;
                        }
                }
                else if(command_str[i][0]!='&')
                {
                        directory[dir_count++] = i;
                }
                ++i;
        }

        if(dir_count == 0)
        {
                no_path = 1;
                dir_count = 1;
        }

        for(int i = 0; i<dir_count; ++i) {
                ls_blk_size = 0;
                if(ls_flag[1] == 0)
                {

                        char pathname[500];
                        int flag = 0;

                        if (ls_flag[0] == 1)
                                flag = 1;

                        if(no_path == 1)
                        {
                                if(!getcwd(pathname, sizeof(pathname)))
                                        printf("Error getting pathname\n");
                        }
                        else
                        {
                                strcpy(pathname, command_str[directory[i]]);
                        }
                        struct dirent **namelist;

                        if(flag == 0)
                                items = scandir(pathname, &namelist, hide, alphasort);
                        else
                                items = scandir(pathname,&namelist,NULL,hiddensort);

                        if(items == -1)
                        {
                          perror("ls");
                          return;
                        }
                        while (items--)
                        {
                                printf("%s ",namelist[items]->d_name);
                        }


                        free(namelist);
                        printf("\n");
                }

                else
                {
                        int flag = 0;

                        if (ls_flag[0] == 1)
                                flag = 1;

                        char pathname[500];

                        if(no_path == 1)
                        {
                                if(!getcwd(pathname, sizeof(pathname)))
                                        printf("Error getting pathname\n");
                        }
                        else
                        {
                                strcpy(pathname, command_str[directory[i]]);
                        }

                        struct dirent **namelist;
                        int items;

                        if(flag == 0)
                                items = scandir(pathname, &namelist, hide, alphasort);
                        else
                                items = scandir(pathname,&namelist,NULL,hiddensort);

                        if(items<0)
                                perror("Error");

                        else
                        {
                                printf("%s:\n",pathname);

                                while(items--)
                                {

                                        char temp[500];
                                        strcpy(temp,pathname);
                                        strcat(temp,"/");
                                        strcat(temp,namelist[items]->d_name);
                                        ls_blk_size +=ls_l(temp,namelist[items]->d_name);

                                }
                                printf("total %d\n",ls_blk_size/2);
                        }
                        free(namelist);
                }
        }

}
