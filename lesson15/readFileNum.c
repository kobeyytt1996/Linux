#include <sys/types.h>
       #include <dirent.h>
       #include <stdio.h>
       #include <string.h>
       #include <stdlib.h>

int getFileNum(const char * path);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("%s path\n", argv[0]);
        return -1;
    }

    printf("%d files in %s\n", getFileNum(argv[1]), argv[0]);
    return 0;
}

int getFileNum(const char * path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        exit(0);
    }
    int num = 0;
    struct dirent *file;
    while ((file = readdir(dir)) != NULL) {
        const char *dname = file->d_name;
        if (strcmp(dname, ".") == 0 || strcmp(dname, "..") == 0) {
            continue;
        }
        
        if (file->d_type == DT_DIR) {
            char newpath[256];
            sprintf(newpath, "%s/%s", path, dname);
            num += getFileNum(newpath);
        }

        if (file->d_type == DT_REG) {
            ++num;
        }
    }
    closedir(dir);
    return num;
}