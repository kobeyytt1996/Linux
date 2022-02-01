#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

int main(int argc, char * argv[]) {

    if (argc < 2) {
        printf("%s filename\n", argv[0]);
        return -1;
    }

    struct stat fileStat;
    int ret = stat(argv[1], &fileStat);
    if (ret == -1) {
        perror("yuan");
        return -1;
    }

    char perms[11] = {0};
    switch (fileStat.st_mode & S_IFMT)
    {
    case S_IFDIR:
        perms[0] = 'd';
        break;
    case S_IFREG:
        perms[0] = '-';
        break;
    default:
        perms[0] = '?';
        break;
    }

    perms[1] = (fileStat.st_mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (fileStat.st_mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (fileStat.st_mode & S_IXUSR) ? 'x' : '-';

    perms[4] = (fileStat.st_mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (fileStat.st_mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (fileStat.st_mode & S_IXGRP) ? 'x' : '-';

    perms[7] = (fileStat.st_mode & S_IROTH) ? 'r' : '-';
    perms[8] = (fileStat.st_mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (fileStat.st_mode & S_IXOTH) ? 'x' : '-';

    const char *filename = getpwuid(fileStat.st_uid)->pw_name;
    const char *groupname = getgrgid(fileStat.st_gid)->gr_name;

    long int fileSize = fileStat.st_size; 
    char * time = ctime(&(fileStat.st_mtim.tv_sec));

    char formatTime[512];
    strncpy(formatTime, time, strlen(time)-1);
    
    char buf[1024] = {0};
    sprintf(buf, "%s %ld %s %s %ld %s %s", perms, fileStat.st_nlink, filename, groupname, fileSize, formatTime, argv[1]);

    printf("%s\n", buf);
    return 0;
}