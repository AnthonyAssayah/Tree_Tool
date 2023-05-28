#define _XOPEN_SOURCE 700
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <string.h>

static int printTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s directory-path\n", argv[0]);
        return 1;
    }

    if (nftw(argv[1], printTree, 10, FTW_PHYS) == -1) {
        perror("nftw");
        return 1;
    }

    return 0;
}

static void printPermissions(mode_t mode) {
    printf("%s", "[");
    putchar((mode & S_IRUSR) ? 'r' : '-');
    putchar((mode & S_IWUSR) ? 'w' : '-');
    putchar((mode & S_IXUSR) ? 'x' : '-');
    putchar((mode & S_IRGRP) ? 'r' : '-');
    putchar((mode & S_IWGRP) ? 'w' : '-');
    putchar((mode & S_IXGRP) ? 'x' : '-');
    putchar((mode & S_IROTH) ? 'r' : '-');
    putchar((mode & S_IWOTH) ? 'w' : '-');
    putchar((mode & S_IXOTH) ? 'x' : '-');
}

static void printOwner(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    if (pw != NULL)
        printf("%s", pw->pw_name);
    else
        printf("%d", uid);
}

static void printGroup(gid_t gid) {
    struct group *gr = getgrgid(gid);
    if (gr != NULL)
        printf("%s", gr->gr_name);
    else
        printf("%d", gid);
}

static int printTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb) {
    for (int i = 0; i < ftwb->level; i++)
        printf("│   ");
    printf("└──");

    if (ftwb->level > 0)
        printf(" ");
    else
        printf("│");

    printPermissions(sbuf->st_mode);
    printf(" ");
    printOwner(sbuf->st_uid);
    printf(" ");
    printGroup(sbuf->st_gid);
    printf("%8ld]", (long) sbuf->st_size);

    printf("    %s\n", &pathname[ftwb->base]);

    return 0;
}
