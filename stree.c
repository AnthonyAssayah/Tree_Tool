#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>


// usefull links: https://github.com/kddnewton/tree/blob/main/tree.c

typedef struct {
  size_t dirs;
  size_t files;
} Counter;

typedef struct Entry {
  char *name;
  int isDir;
  struct Entry *next;
} Entry;

// Function to print the file permissions
void printPermissions(mode_t mode) {
  printf("[");
  putchar((mode & S_IFDIR) ? 'd' : '-');
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

// Function to print the user and group information
void printUserGroup(uid_t uid, gid_t gid) {
  struct passwd *pw = getpwuid(uid);
  struct group *gr = getgrgid(gid);
  if (pw != NULL && gr != NULL) {
    printf(" %s  %s", pw->pw_name, gr->gr_name);
  }
}

// Function to traverse files and directories recursively
int traverseFiles(const char* directory, const char* prefix, Counter *counter) {
  Entry *head = NULL, *current, *iter;
  size_t size = 0;

  struct dirent *fileDir;
  DIR *dir;

  char *segment, *pointer, *nextPrefix;

  dir = opendir(directory);
  if (!dir) {
    fprintf(stderr, "Cannot open directory \"%s\"\n", directory);
    return -1;
  }

  counter->dirs++;

  // Read the directory entries
  while ((fileDir = readdir(dir)) != NULL) {
    if (fileDir->d_name[0] == '.') {
      continue;  // Skip hidden files/directories
    }

    // Create a new entry for the file/directory
    current = malloc(sizeof(Entry));
    current->name = strdup(fileDir->d_name);
    current->isDir = fileDir->d_type == DT_DIR;
    current->next = NULL;

    // Insert the entry in sorted order
    if (head == NULL || strcmp(current->name, head->name) < 0) {
      current->next = head;
      head = current;
    } else {
      iter = head;
      while (iter->next && strcmp(current->name, iter->next->name) > 0) {
        iter = iter->next;
      }
      current->next = iter->next;
      iter->next = current;
    }

    size++;
  }

  closedir(dir);
  if (!head) {
    return 0;
  }

  // Traverse the sorted entries
  for (size_t index = 0; index < size; index++) {
    if (index == size - 1) {
      pointer = "└── ";
      segment = "    ";
    } else {
      pointer = "├── ";
      segment = "│    ";
    }

    printf("%s%s", prefix, pointer);

    struct stat fileStat;
    char* fullPath = malloc(strlen(directory) + strlen(head->name) + 2);
    sprintf(fullPath, "%s/%s", directory, head->name);

    if (lstat(fullPath, &fileStat) != -1) {
      printPermissions(fileStat.st_mode);
      printf(" ");
      printUserGroup(fileStat.st_uid, fileStat.st_gid);
      printf(" %8ld]", (long) fileStat.st_size);
      printf("  %s\n", head->name);

      if (head->isDir) {
        nextPrefix = malloc(strlen(prefix) + strlen(segment) + 1);
        sprintf(nextPrefix, "%s%s", prefix, segment);

        traverseFiles(fullPath, nextPrefix, counter);
        free(nextPrefix);
      } else {
        counter->files++;
      }
    }

    free(fullPath);

    current = head;
    head = head->next;

    free(current->name);
    free(current);
  }

  return 0;
}

int main(int argc, char *argv[]) {
  const char* directory = (argc > 1) ? argv[1] : ".";
  printf("%s\n", directory);

  Counter counter = {0, 0};
  traverseFiles(directory, "", &counter);

  printf("\n%zu directories, %zu files\n",
         (counter.dirs > 0) ? counter.dirs - 1 : 0, counter.files);
  return 0;
}
