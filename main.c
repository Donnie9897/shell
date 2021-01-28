#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LENGTH 1000
#define MAX_ARG_COUNT 100

/*
 * READ
 * EVAL
 * PRINT
 * LOOP
 * (REPL)
*/

int cmdHandler(char** cmd) {    
    // Declarations
    const int amountOfCommands = 2;
    char * cmds[2] = { "exit", "hello" };
    int commandId = 0;
    int idx = 0;
    char* username;

    // Evaluate if input is a known command
    for(; idx < amountOfCommands; idx++) {
        if(strcmp(cmd[0], cmds[idx]) == 0) {
            commandId = idx + 1;
            break;
        }
    }

    switch (commandId)
    {
    case 1:
        exit(0);
        break;    
    case 2:        
        username = getenv("USER"); // Retrieve USER environment variable
        printf("Hello %s!\n", username);
        return 1;
    default:                
        break;
    }

    return 0;
}

void execute(char** input) {
    // fork child process
    pid_t pid = fork();

    if(pid == -1) {
        printf("Forking child failed...");
        return;
    } else if (pid == 0) {
        if(execvp(input[0], input) < 0) {
            printf("shell: command not found: %s\n", input[0]);
        }
        exit(0);
    } else {
        wait(NULL);
        return;
    }
}

// void executePiped(char** parsedArgs, char** parsedArgs)

int capture(char * str) {
    char * buffer;
    // Capture user input in buffer
    buffer = readline("> ");
    if(strlen(buffer) != 0) {
        // TODO: RR: keep track of commands or instructions sent via the shell

        // Copy buffer to input variable sent via str param
        strcpy(str, buffer);

        // Return 0 when we have captured something
        return 1;
    } else {
        // Return 1 when we have not captured something and need to READ again.
        return 0;
    }
}

int isPiped(char* str, char** separated) {
    int idx = 0;    
    for(; idx < 2; idx++) {
        separated[idx] = strsep(&str, "|");
        if(separated[idx] == NULL)
            break;
    }

    // False or zero when pipe is not found
    if(separated[1] == NULL) return 0;

    return 1;
}

void spaceParser(char* str, char** parsed) {    
    int idx = 0;
    for(; idx < MAX_ARG_COUNT; idx++) {
        parsed[idx] = strsep(&str, " ");

        if(parsed[idx] == NULL) break;
        if(strlen(parsed[idx]) == 0) idx--;
    }    
}

int process(char * str, char** parsedArgs, char** parsedArgsPiped) {
    char* striped[2];
    int piped = 0;

    piped = isPiped(str, striped);

    if(piped) {
        spaceParser(striped[0], parsedArgs);
        spaceParser(striped[1], parsedArgsPiped);
    } else {
        spaceParser(str, parsedArgs);
    }

    if(cmdHandler(parsedArgs)) {
        return 0;
    } else {
        return 1 + piped;
    }
}

int main(int argc, char * argv[])
{
    char input[MAX_LENGTH];
    char * parsedArgs[MAX_ARG_COUNT];
    char * parsedArgsPiped[MAX_ARG_COUNT];
    int executionFlag = -1;
    while(1) {        
        // take input
        if(!capture(input)) 
            continue; // Reset loop and attempt to capture input again

        // process (eval)
        // execute
        // print
        executionFlag = process(input, parsedArgs, parsedArgsPiped);

        // TODO: RR: execution flag must be evaluated in order to support piping
        if(executionFlag == 1) {
            execute(parsedArgs);
        }

        // if(executionFlag == 2) {
        //     executePiped(parsedArgs, parsedArgsPiped);
        // }

        // continue loop
    }

    return 0;
}
