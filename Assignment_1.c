#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <ftw.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <libgen.h>

#include <unistd.h>

char ** globalargv;
int target = -1;
char * startpath = ".";
int count = 0;

int customNftw(const char * fname,
  const struct stat * statptr, int fileflags, struct FTW * pfwt) {
  if (fileflags == FTW_SL) { // if it is a symbolic link
    char * fullpath = realpath(fname, NULL); // character pointer to the full path of the file from the symbolic link
    char * dname = dirname(fullpath); // character pointer to the directory name to check if real path matches any entry in the ancestry.
    if (strlen(dname) == strlen(startpath)) {
      int flag = 0;
      for (int i = 0; i < strlen(dname); i++) {
        if (dname[i] != startpath[i]) {
          flag = 1;
          printf("%s\n", fname); // match is found, print the whole pathname of link
          break;
        }
      }
      count++;
    }
  }
  return 0;
}

int main(int argc, char * argv[]) {
  globalargv = argv;
  char * currentWorkingDir = getcwd(NULL, 0);
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "-t") == 0) {
      target = i;
    }
  }
  if (target <= -1) {
    target = argc;
    startpath = currentWorkingDir;
  } else {
    startpath = argv[target + 1];

  }
  int fd_limit = 99;
  int flags = FTW_PHYS; // FTW_PHYS to perform a physical walk 
  int ret;
  ret = nftw(startpath, customNftw, fd_limit, flags);
  printf("%d\n", count);
}
