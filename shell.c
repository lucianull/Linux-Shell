#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>

#define NMAX 1024

bool RUN = true;
char currentPath[NMAX], **shellHistory, *command;
int shellHistorySize = -1, error_code; // de initializat dimensiunea maxima

void ExecuteClear()
{
    const char escapeSeq[7] = "\33[H\33[2J";
    printf(escapeSeq);
}

void ExecuteCommands(char **command, int nrArgs)
{
    if(!strcmp(command[0], "clear"))
    {
        if(nrArgs != 1)
        {
            error_code = 1; // too many args
        }
        ExecuteClear()
    }
}

int main(int argc, char *argv[])
{
    int N;
    char *Pointer;
    command = malloc(sizeof(char) * NMAX);
    while(RUN)
    {
        if(getcwd(currentPath, sizeof(currentPath)) != NULL)
            printf("SHELL:%s$ ", currentPath);
        else
            error_code = 1;
        scanf("%s", command);
        if(!strcmp(command, ""))
        {
            printf("\n");
            continue;
        }
        shellHistorySize++;
        shellHistory[shellHistorySize] = malloc(sizeof(char) * strlen(command));
        strcpy(shellHistory[shellHistorySize], command);

        Pointer = strtok(command, " ");
        while(Pointer != NULL)
        {

            Pointer = strtok(NULL, " ");
        }

    }
    return 0;
}