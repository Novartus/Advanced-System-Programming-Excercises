#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <ftw.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <libgen.h>

#include <unistd.h>

char ** globalArgV;
int target = -1;
char * startPath = ".";
int count = 0;

int customNftw(const char * fName,
  const struct stat * statPtr, int fileFlags, struct FTW * pfwt) {
  if (fileFlags == FTW_SL) { // if it is a symbolic link
    char * fullpath = realpath(fName, NULL); // character pointer to the full path of the file from the symbolic link
    char * dName = dirname(fullpath); // character pointer to the directory name to check if real path matches any entry in the ancestry.
    
    if (strlen(dName) == strlen(startPath)) {
      int flag = 0;
      for (int i = 0; i < strlen(dName); i++) {
        if (dName[i] != startPath[i]) {
          flag = 1;
          printf("%s\n", fName); // match is found, print the whole pathname of link
          count++;
          break;
        }
      }
    }
  }
  return 0;
}

int main(int argc, char * argV[]) {
  globalArgV = argV;
  char * currentWorkingDir = getcwd(NULL, 0);
  for (int i = 0; i < argc; i++) {
    if (argc == 2) {
      target = i;
    }
  }

  if (target <= -1) {
    target = argc;
    startPath = currentWorkingDir;
  } else {
    startPath = argV[target];
  }
  int fd_limit = 99;
  int flags = FTW_PHYS; // FTW_PHYS to perform a physical walk 
  int ret;
  ret = nftw(startPath, customNftw, fd_limit, flags);
  printf("%d\n", count);
}
