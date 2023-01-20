#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>

#define NMAX 1024

bool RUN = true;
char currentPath[NMAX], **shellHistory, *command, **buffer;
int shellHistorySize = -1, error_code, bufferSize; // de initializat dimensiunea maxima

void ParseError()
{
}

void ExecuteHelp()
{
    printf("\nCommands available:\n");
    printf("--------------------------------------------------------\n\n");
    printf("history: prints the command history of the current session \n");        // 
    printf("clear: clears the terminal \n");                                        // 
    printf("cd: changes the path to a particular directory \n");                    //
    printf("pwd: prints the path of the current directory \n");
    printf("ls: lists all files and directories in the current directory \n");
    printf("touch: creates a new empty file \n");
    printf("rm: deletes a particular file \n");
    printf("cp: copies the content of a file to another file \n");
    printf("makedir: creates a new directory \n");
    printf("removedir: deletes an already existing directory \n");
    printf("echo: displays a string that is passed as an argument\n");              //
    printf("quit: exits the shell \n");                                             //

    printf("\n-----------------------------------------------------\n\n");
}

void ExecuteEcho(char **callStack, int *nrArgs)
{
    for (int i = 1; i <= *nrArgs; i++)
        bufferSize++, strcpy(buffer[bufferSize], callStack[i]);
    *nrArgs = -1;
}

void ExecuteHistory()
{
    for (int i = 0; i <= shellHistorySize; i++)
        printf("%s", shellHistory[i]);
}

void ExecuteCd(char *folder)
{
    if(chdir(folder))
    {
        error_code = 2;
    }
}

void ExecuteCommands(char **command, int *nrArgs)
{
    if (!strcmp(command[0], "clear"))
    {
        if (nrArgs)
        {
            error_code = 1; // too many args
        }
        else
            printf("\33[H\33[2J");
    }
    else
    {
        if (!strcmp(command[0], "quit"))
        {
            if (*nrArgs)
                error_code = 1;
            else
                RUN = false;
        }
        else
        {
            if (!strcmp(command[0], "help"))
            {
                if (*nrArgs)
                    error_code = 1;
                else
                    ExecuteHelp();
            }
            else
            {
                if (!strcmp(command[0], "echo"))
                {
                    if(!*nrArgs)
                        error_code = 1;
                    else
                        ExecuteEcho(command, nrArgs);
                }
                else
                {
                    if (!strcmp(command[0], "history"))
                    {
                        if(*nrArgs)
                            error_code = 1;
                        else
                            ExecuteHistory();
                    }
                    else
                    {
                        if(!strcmp(command[0], "cd"))
                        {
                            if(*nrArgs > 1)
                                error_code = 1;
                            else
                            {
                                if(*nrArgs)
                                    ExecuteCd(command[1]);
                                else
                                    ExecuteCd("..");
                            }
                        }
                    }
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    int N, callStackSize;
    char *Pointer, **callStack, *aux;

    shellHistory = (char **)malloc(sizeof(char *) * NMAX);
    callStack = (char **)malloc(sizeof(char *) * NMAX);
    buffer = (char **)malloc(sizeof(char *) * NMAX);
    for (int i = 0; i < NMAX; i++)
    {
        callStack[i] = (char *)malloc(sizeof(char) * NMAX);
        buffer[i] = (char *)malloc(sizeof(char) * NMAX);
    }

    while (RUN)
    {
        if (getcwd(currentPath, sizeof(currentPath)) != NULL)
            printf("SHELL:~%s$ ", currentPath);
        else
            error_code = 2;

        error_code = 0;
        command = NULL;
        size_t length = 0;
        ssize_t lineLength = 0;
        bufferSize = -1;
        lineLength = getline(&command, &length, stdin);
        if (!strcmp(command, ""))
        {
            printf("\n");
            continue;
        }
        shellHistorySize++;
        shellHistory[shellHistorySize] = (char *)malloc(sizeof(char) * strlen(command));
        strcpy(shellHistory[shellHistorySize], command);
        callStackSize = -1;
        Pointer = strtok(command, " \n");
        while (Pointer != NULL)
        {
            if (!strcmp(Pointer, "|"))
            {
                ExecuteCommands(callStack, &callStackSize);
                if (error_code)
                {
                    ParseError();
                    break;
                }
                Pointer = strtok(NULL, " \n");
                strcpy(callStack[0], Pointer);
                for (int i = 0; i <= bufferSize; i++)
                    strcpy(callStack[i + 1], buffer[i]);
                callStackSize = bufferSize + 1;
                bufferSize = -1;
            }
            else
            {
                if (!strcmp(Pointer, "&&"))
                {
                    ExecuteCommands(callStack, &callStackSize);
                    if (error_code)
                    {
                        ParseError();
                        break;
                    }
                }
                else
                {
                    if (!strcmp(Pointer, "||"))
                    {
                        ExecuteCommands(callStack, &callStackSize);
                        if (error_code)
                            continue;
                    }
                    else
                    {
                        callStackSize++;
                        strcpy(callStack[callStackSize], Pointer);
                    }
                }
            }
            Pointer = strtok(NULL, " \n");
        }
        ExecuteCommands(callStack, &callStackSize);
        if (error_code)
            ParseError();
        else
        {
            for (int i = 0; i <= bufferSize; i++)
                printf("%s ", buffer[i]);
            printf("\n");
        }
        free(command);
    }
    return 0;
}
