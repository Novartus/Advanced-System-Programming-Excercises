#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

int readCommand(int fd, char *line, int size)
{
    int i = 0, n;
    char ch;
    do
    {
        n = read(fd, &ch, 1);
        if (!n)
        {
            break;
        }
        if (ch == '\n' || ch == ';') // Deal with 'new line', ';'
        {
            break;
        }
        line[i] = ch;
        i++;
    } while (i < size);
    line[i] = '\0';
    return i;
}

void parent(int *fd, char *args[])
{ // A writer
    close(fd[0]);
    dup2(fd[1], 1); // 1 is the standard output
    close(fd[1]);   // close original file descriptor
    execvp(args[0], args);
}
void child(int *fd, char *args[])
{ // A reader
    close(fd[1]);
    dup2(fd[0], 0); // 0 is the standard input
    close(fd[0]);   // close original file descriptor
    execvp(args[0], args);
}

void argsBeforeRedirect(char **args, int indexRedirect)
{
    char **argvBeforeRedirect = malloc(sizeof(char *) * (indexRedirect + 1));
    for (int i = 0; i < indexRedirect; i++)
    { // loop to add all argv in argvBeforeRedirect
        argvBeforeRedirect[i] = args[i];
    }
    argvBeforeRedirect[indexRedirect] = NULL;
    int fd = open(args[indexRedirect + 1], O_WRONLY | O_CREAT, 0644); // redirect output to file
    dup2(fd, 1);
    execvp(args[0], argvBeforeRedirect);
    close(fd);
}

void handlePipe(char **args, int indexPipe)
{
    char **argvBeforePipe = malloc(sizeof(char *) * (indexPipe + 1));
    for (int i = 0; i < indexPipe; i++)
    { // loop to add all argv in argvBeforePipe
        argvBeforePipe[i] = args[i];
    }
    argvBeforePipe[indexPipe] = NULL;

    char **argsAfterPipe = malloc(sizeof(char *) * (sizeof(args) - indexPipe));
    int afterPipeSize = 0;
    for (afterPipeSize = indexPipe + 1; afterPipeSize < sizeof(args); afterPipeSize++)
    { // loop to add all argv in argvBeforePipe
        if (args[afterPipeSize] == NULL)
            break;
        argsAfterPipe[afterPipeSize - indexPipe - 1] = args[afterPipeSize];
    }
    argsAfterPipe[afterPipeSize] = NULL;

    // creating pipe
    int pd[2];
    if (pipe(pd) == -1)
        exit(1);
    pid_t pid = fork();

    if (pid > 0)
    {
        parent(pd, argvBeforePipe);
    }
    else if (pid == 0)
    {
        child(pd, argsAfterPipe);
    }
    else
    {
        perror("Fork Error in piping\n");
        exit(1);
    }
}

void main()
{
    char line[255], delim[4] = " \0"; // blank or NULL
    char **vectorArgument = malloc(10 * sizeof(char *));
    if (readCommand(0, line, 255) > 0)
    {
        int i = 0;
        vectorArgument[i++] = strtok(line, delim); // get first string
        do
        {
            vectorArgument[i++] = strtok(NULL, delim);
        } while (vectorArgument[i - 1] != NULL);
        // fork for each command in vectorArgument
        if (vectorArgument[1] == NULL) // single argument command
        {
            pid_t pid = fork();
            if (pid == -1)
            { // error
                char *error = strerror(errno);
                printf("Fork Error\n");
                return;
            }
            else if (pid == 0) // child process
            {
                execvp(vectorArgument[0], vectorArgument); // run command in vectorArgument
                char *error = strerror(errno);
                printf("Unknown Command\n");
                return;
            }
            else
            { // parent process
                int childStatus;
                waitpid(pid, &childStatus, 0);
                return;
            }
        }
        else if (vectorArgument[1] != NULL) // check for multiple arguments
        {
            // check if it contains pipe or not
            int indexPipe = 0;
            for (int i = 0; i < 255; i++)
            {
                if (vectorArgument[i] == NULL)
                {
                    break;
                }
                if (strcmp(vectorArgument[i], "|") == 0)
                {
                    indexPipe = i;
                    break;
                }
            }
            // if it contains pipe the execute pipe process
            if (indexPipe != 0)
            {
                handlePipe(vectorArgument, indexPipe);
            }
            else
            {
                pid_t pid = fork();
                if (pid == -1)
                { // error
                    char *error = strerror(errno);
                    printf("Fork Error\n");
                    return;
                }
                else if (pid == 0)
                { // child process

                    int indexRedirect = 0; // index of '>' in args
                    for (int i = 0; i < 255; i++)
                    {
                        if (vectorArgument[i] == NULL)
                        {
                            break;
                        }
                        if (strcmp(vectorArgument[i], ">") == 0)
                        {
                            indexRedirect = i;
                            break;
                        }
                    }
                    // Output redirection to file
                    if (indexRedirect != 0)
                    {
                        argsBeforeRedirect(vectorArgument, indexRedirect);
                    }
                    else
                    {
                        execvp(vectorArgument[0], vectorArgument);
                    }
                    // char *error = strerror(errno);
                    // printf("Unknown Command\n");
                    return;
                }
                else
                { // parent process
                    int childStatus;
                    waitpid(pid, &childStatus, 0);
                    return;
                }
            }
        }
        else
        {
            printf("Unknown Command\n");
            exit(1);
        }
    }
}