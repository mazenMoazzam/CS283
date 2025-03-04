#include "dshlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <fcntl.h>

char* skipWhiteSpace(char* commandLineString) {
    while (*commandLineString && isspace((unsigned char)*commandLineString)) {
        commandLineString++;
    }
    return commandLineString;
}

char* processQuoteStrings(char* commandLineString, cmd_buff_t *cmd_buff) {
    commandLineString++;
    cmd_buff->argv[cmd_buff->argc++] = commandLineString;
    
    while (*commandLineString && *commandLineString != '\"') {
        commandLineString++;
    }
    
    if (*commandLineString == '\"') {
        *commandLineString = '\0'; 
        commandLineString++;
    }

    return commandLineString;
}

char* processUnquotedStrings(char* commandLineString, cmd_buff_t *cmd_buff) {
    cmd_buff->argv[cmd_buff->argc++] = commandLineString;

    while (*commandLineString != '\0' && !isspace((unsigned char)*commandLineString)) {
        commandLineString++;
    }

    if (*commandLineString != '\0') {
        *commandLineString = '\0'; 
        commandLineString++;
    }

    return commandLineString;
}

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }

    memset(cmd_buff, 0, sizeof(cmd_buff_t));
    return OK;
}

int free_cmd_buff(cmd_buff_t *cmd_buff) {
    if (cmd_buff->_cmd_buffer != NULL) {
        free(cmd_buff->_cmd_buffer);
    }
    return OK;
}

int clear_cmd_buff(cmd_buff_t *cmd_buff) {
    memset(cmd_buff, 0, sizeof(cmd_buff_t));
    return OK;
}

int build_cmd_buff(char *cmd_line, cmd_buff_t *cmd_buff) {
    if (!cmd_line || strlen(cmd_line) >= SH_CMD_MAX) {
        return ERR_CMD_OR_ARGS_TOO_BIG;
    }

    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer) {
        return ERR_MEMORY;
    }

    cmd_buff->argc = 0;
    cmd_buff->inputFile = NULL;
    cmd_buff->outputFile = NULL;
    cmd_buff->outputAppend = 0;

    char *commandLineStringPointer = cmd_buff->_cmd_buffer;

    while (*commandLineStringPointer != '\0') {
        commandLineStringPointer = skipWhiteSpace(commandLineStringPointer);

        if (*commandLineStringPointer == '\0') {
            break;
        }

        
        if (*commandLineStringPointer == '<') {
            *commandLineStringPointer = '\0';
            commandLineStringPointer++;
            commandLineStringPointer = skipWhiteSpace(commandLineStringPointer);
            cmd_buff->inputFile = commandLineStringPointer;

            
            while (*commandLineStringPointer && !isspace(*commandLineStringPointer)) {
                commandLineStringPointer++;
            }
            if (*commandLineStringPointer) {
                *commandLineStringPointer = '\0';
                commandLineStringPointer++;
            }
            continue;
        }


        if (*commandLineStringPointer == '>') {
            *commandLineStringPointer = '\0';
            commandLineStringPointer++;
            if (*commandLineStringPointer == '>') {
                cmd_buff->outputAppend = 1; 
                commandLineStringPointer++;
            } else {
                cmd_buff->outputAppend = 0; 
            }
            commandLineStringPointer = skipWhiteSpace(commandLineStringPointer);
            cmd_buff->outputFile = commandLineStringPointer;

           
            while (*commandLineStringPointer && !isspace(*commandLineStringPointer)) {
                commandLineStringPointer++;
            }
            if (*commandLineStringPointer) {
                *commandLineStringPointer = '\0';
                commandLineStringPointer++;
            }
            continue;
        }

    
        if (*commandLineStringPointer == '\"') {
            commandLineStringPointer = processQuoteStrings(commandLineStringPointer, cmd_buff);
        } else {
            commandLineStringPointer = processUnquotedStrings(commandLineStringPointer, cmd_buff);
        }

        if (cmd_buff->argc >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }
    }

    cmd_buff->argv[cmd_buff->argc] = NULL;
    return OK;
}


Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(input, "dragon") == 0) {
        return BI_CMD_DRAGON;
    } else if (strcmp(input, "cd") == 0) {
        return BI_CMD_CD;
    } else {
        return BI_NOT_BI;
    }
}

Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    switch (match_command(cmd->argv[0])) {
        case BI_CMD_EXIT:
	    printf("exiting...\n");
            return BI_CMD_EXIT;
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd failed");
                    return ERR_EXEC_CMD;
                }
            }
            return BI_EXECUTED;

        case BI_CMD_DRAGON:
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int handleInputRedirection(cmd_buff_t *cmd) {
    if (cmd->inputFile != NULL) {
        int inputFileDirection = open(cmd->inputFile, O_RDONLY);
        if (inputFileDirection < 0) {
            perror("Failed to open the input file");
            exit(ERR_EXEC_CMD);
        }
        dup2(inputFileDirection, STDIN_FILENO);
        close(inputFileDirection);
    }
    return OK;
}


int handleOutputRedirection(cmd_buff_t *cmd) {
    if (cmd->outputFile != NULL) {
        int outputFileMode = O_WRONLY | O_CREAT;
        if (cmd->outputAppend) {
            outputFileMode |= O_APPEND;
        } else {
            outputFileMode |= O_TRUNC;
        }
        int outputFileDirection = open(cmd->outputFile, outputFileMode, FILE_PERMISSIONS);
        if (outputFileDirection < 0) {
            perror("Failed to open the file");
            exit(ERR_EXEC_CMD);
        }
        dup2(outputFileDirection, STDOUT_FILENO);
        close(outputFileDirection);
    }
    return OK;
}


void executeProcess(cmd_buff_t *cmd) {
    handleInputRedirection(cmd);
    handleOutputRedirection(cmd);
    execvp(cmd->argv[0], cmd->argv);
    perror("Execvp has failed");
    exit(ERR_EXEC_CMD);
}



int exec_cmd(cmd_buff_t *cmd) {
    if (exec_built_in_cmd(cmd) == BI_EXECUTED) {
        return OK;
    }
    pid_t pid = fork();
    if (pid == 0) {
        executeProcess(cmd);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
    } else {
        perror("Fork process has failed");
        return ERR_EXEC_CMD;
    }
    return OK;
}


int execute_pipeline(command_list_t *clist) {
    int numberOfCommands = clist->num;
    int pipeFileDescriptors[2];
    int previousPipeRead = -1;
    pid_t pids[numberOfCommands];

    for (int i = 0; i < numberOfCommands; i++) {
        if (i < numberOfCommands - 1) {
            if (pipe(pipeFileDescriptors) == -1) {
                perror("Pipe process failed");
                return ERR_EXEC_CMD;
            }
        }

        pid_t pid = fork();
        if (pid == 0) {
            if (i > 0) {
                dup2(previousPipeRead, STDIN_FILENO);
                close(previousPipeRead);
            }

            if (i < numberOfCommands - 1) {
                dup2(pipeFileDescriptors[1], STDOUT_FILENO);
                close(pipeFileDescriptors[1]);
                close(pipeFileDescriptors[0]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("Execvp process has failed");
            exit(ERR_EXEC_CMD);
        } else if (pid > 0) {
            pids[i] = pid;
            if (i > 0) {
                close(previousPipeRead);
            }
            if (i < numberOfCommands - 1) {
                previousPipeRead = pipeFileDescriptors[0];
                close(pipeFileDescriptors[1]);
            }
        } else {
            perror("Fork process has failed");
            return ERR_EXEC_CMD;
        }
    }
    
    for (int i = 0; i < numberOfCommands; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }
    return OK;
}



int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    command_list_t clist;
    int status = OK;

    while (1) {
        printf(SH_PROMPT);
        
        if (!fgets(input, SH_CMD_MAX, stdin)) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (build_cmd_list(input, &clist) == OK) {
            if (clist.num == 1) {
                if (exec_built_in_cmd(&clist.commands[0]) == BI_CMD_EXIT) {
                    status = 0; 
                    break;
                }
                status = exec_cmd(&clist.commands[0]);
            } else {
                status = execute_pipeline(&clist);
            }
        }
    }
    return status; 
}






int build_cmd_list(char *cmd_line, command_list_t *clist) {
    char *command;
    int commandCount = 0;
    clist->num = 0;
    memset(clist->commands, 0, sizeof(clist->commands));

    if (cmd_line == NULL || strlen(cmd_line) == 0 || strspn(cmd_line, " ") == strlen(cmd_line)) {
        return WARN_NO_CMDS;
    }

    command = strtok(cmd_line, PIPE_STRING);

    while (command != NULL) {
        char *endOfString = command + strlen(command) - 1;

        while (*command != '\0' && isspace(*command)) {
            command++;
        }

        while (endOfString > command && isspace(*endOfString)) {
            endOfString--;
        }
        *(endOfString + 1) = '\0';

        if (commandCount >= CMD_MAX) {
            return ERR_TOO_MANY_COMMANDS;
        }

 
        if (alloc_cmd_buff(&clist->commands[commandCount])) {
            return ERR_MEMORY;
        }
   
        if (build_cmd_buff(command, &clist->commands[commandCount])) {
            return ERR_CMD_OR_ARGS_TOO_BIG;
        }

        commandCount++;
        command = strtok(NULL, PIPE_STRING);
    }

    clist->num = commandCount;
    return OK;
}
