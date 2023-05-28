#define _XOPEN_SOURCE 600
#include <ftw.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>

typedef struct node
{
    int level, last, indent;
    int *dirs;
    struct node *prev;
    struct node *next;
} node;

node *head;
node *curr;
char *ENTRY = "├── ";
char *LAST = "└── ";
int n_files = 0, n_dirs = 0, prefix = 0, max_lvl = 0;
char *perms[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};

static int list(const char *name, const struct stat *status, int type, struct FTW *ftwb)
{
    if (type == FTW_NS)
        return 0;

    if (type == FTW_F)
    {
        curr = curr->next;
        struct passwd *pwd;
        struct group *grp;
        pwd = getpwuid(status->st_uid);
        grp = getgrgid(status->st_gid);
        char a[4];
        sprintf(a, "%3o", status->st_mode & 0777);
        ++n_files;
        if (curr->next == NULL)
        {
            curr->last = 1;
        }
        else
        {
            node *ptr = curr->next;
            while (ptr != NULL && ptr->next != NULL && ptr->level > curr->level)
            {
                if (ptr->next->next == NULL)
                    curr->last = 1;

                ptr = ptr->next;
            }
            if (ptr->level < curr->level)
            {
                curr->last = 1;
            }
        }
        for (int i = 0; i < ftwb->level - 1; ++i)
        {
            (curr->dirs[i + 1])? printf("    ") : printf("│   ");
        }
        // for (int i = ftwb->level - curr->indent; i < ftwb->level; ++i)
        // {
        //     printf("    ");
        // }
        printf("%s[-%s%s%s %s %s %*ld]  %s\n", curr->last ? LAST : ENTRY, perms[a[0] - '0'], perms[a[1] - '0'], perms[a[2] - '0'], pwd->pw_name, grp->gr_name, 11, status->st_size, name + ftwb->base);
    }

    if (type == FTW_D && strcmp(".", name) != 0 && strcmp("", name) != 0 && strcmp("..", name) != 0 && ftwb->level > 0)
    {
        curr = curr->next;

        struct passwd *pwd;
        struct group *grp;
        pwd = getpwuid(status->st_uid);
        grp = getgrgid(status->st_gid);
        char a[4];
        sprintf(a, "%3o", status->st_mode & 0777);
        ++n_dirs;
        if (curr->next == NULL)
        {
            curr->last = 1;
        }
        else
        {
            node *ptr = curr->next;
            while ( ptr != NULL && ptr->next != NULL && ptr->level > curr->level)
            {
                if (ptr->next->next == NULL && ptr->next->level > curr->level)
                {
                    curr->last = 1;
                }
                ptr = ptr->next;
            }
            if (ptr->level < curr->level)
            {
                curr->last = 1;
            }
        }
        if (curr->last && curr->next != NULL)
        {
            node *ptr = curr->next;
            while(ptr != NULL && ptr->level > curr->level){
                ptr->dirs[curr->level] = 1;
                ptr = ptr->next;
            }
        }
        for (int i = 0; i < ftwb->level - 1; ++i)
        {
            (curr->dirs[i + 1])? printf("    ") : printf("│   ");
        }
        // for (int i = ftwb->level - curr->indent; i < ftwb->level; ++i)
        // {
        //     printf("    ");
        // }
        printf("%s[d%s%s%s %s %s %*ld]  %s\n", curr->last ? LAST : ENTRY, perms[a[0] - '0'], perms[a[1] - '0'], perms[a[2] - '0'], pwd->pw_name, grp->gr_name, 11, status->st_size, name + ftwb->base);
        
    }

    return 0;
}

static int pre_list(const char *name, const struct stat *status, int type, struct FTW *ftwb)
{
    if (type == FTW_NS)
        return 0;
    if ((type == FTW_D && strcmp(".", name) != 0 && strcmp("", name) != 0 && strcmp("..", name) != 0) || type == FTW_F)
    {
        curr->next = (node *)malloc(sizeof(node));
        curr->next->dirs = (int *)calloc(ftwb->level, sizeof(int));
        curr->next->level = ftwb->level;
        curr->next->last = curr->next->indent = 0;
        curr->next->prev = curr;
        curr = curr->next;
        curr->next = NULL;
        if (ftwb->level > max_lvl)
            max_lvl = ftwb->level;
    }
    return 0;
}

int main(int argc, char *argv[])
{
    head = (node *)malloc(sizeof(node));
    head->level = head->last = head->indent = 0;
    curr = head;

    if (argc == 1)
    {
        nftw(".", pre_list, 1, 0);
        curr = head;
        nftw(".", list, 1, 0);
    }
    else
    {
        prefix = strlen(argv[1]);
        nftw(argv[1], pre_list, 1, 0);
        curr = head;
        nftw(argv[1], list, 1, 0);
    }
    while (head != NULL && head->next != NULL)
    {
        curr = head;
        head = head->next;
        free(curr->dirs);
        free(curr);
    }
    free(head->dirs);
    free(head);
    printf("\n%d directories, %d files\n", n_dirs, n_files);
    return 0;
}
