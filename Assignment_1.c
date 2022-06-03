#define _GNU_SOURCE
#define _XOPEN_SOURCE 500

#include <ftw.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <libgen.h>

#include <unistd.h>

#include <stdbool.h>

char **globalArgV;
int target = -1;
char *startPath = ".";
int count = 0;

// KMP algorithm for string matching
void preSufArr(char *pat, int q, int *processPattern)
{
  int len = 0;
  processPattern[0] = 0;
  int l = 1;
  while (l < q)
  {
    if (pat[l] == pat[len])
    {
      len++;
      processPattern[l] = len;
      l++;
    }
    else
    {
      if (len != 0)
        len = processPattern[len - 1];
      else
      {
        processPattern[l] = 0;
        l++;
      }
    }
  }
}

void KMPAlgo(char *txt, char *pattrn,
             const char *fname)
{
  int A = strlen(pattrn);
  int B = strlen(txt);
  int processPattern[A];
  preSufArr(pattrn, A, processPattern);
  int C = 0;
  int D = 0;
  while (C < B)
  {
    if (pattrn[D] == txt[C])
    {
      C++;
      D++;
    }
    if (D == A)
    {
      count++;
      D = processPattern[D - 1];
      printf("%s\n", fname);
    }
    else if (C < B && pattrn[D] != txt[C])
    {
      if (D != 0)
        D = processPattern[D - 1];
      else
        C = C + 1;
    }
  }
}

int customNftw(const char *fName,
               const struct stat *statPtr, int fileFlags, struct FTW *pfwt)
{
  if (fileFlags == FTW_SL)
  { // if it is a symbolic link
    char buf[4096];
    realpath(fName, buf);
    char *file = strdup(fName);
    char buf2[1024];
    ssize_t len = readlink(file, buf2, 1023);
    buf2[len] = '\0';
    KMPAlgo(dirname(file), buf2, fName); // Pattern matching to check if parent or not
  }
  return 0;
}

int main(int argc, char *argV[])
{
  globalArgV = argV;
  char *currentWorkingDir = getcwd(NULL, 0);
  for (int i = 0; i < argc; i++)
  {
    if (argc == 2)
    {
      target = i;
    }
  }

  if (target <= -1)
  {
    target = argc;
    startPath = currentWorkingDir;
  }
  else
  {
    startPath = argV[target];
  }
  int fd_limit = 99;
  int flags = FTW_PHYS; // FTW_PHYS to perform a physical walk
  int ret;
  ret = nftw(startPath, customNftw, fd_limit, flags);
  printf("%d\n", count);
}
