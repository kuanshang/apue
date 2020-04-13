#include "apue.h"
#include <dirent.h>
int main (int argc,char *argv[])
{
    DIR *dp;
    struct dirent *dirp;
    if (argc != 2)
        err_quit("usage:ls directory_name");
    if ((dp = opendir(argv[1])) == 0)
        err_sys("cannot open %s",argv[1]);
    while ((dirp = readdir(dp)) != 0)
        printf("%s\n",dirp->d_name);
    closedir(dp);
    exit(0);
}
