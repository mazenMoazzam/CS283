#include "dshlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>


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
    int isQuoted = 0;
    char *commandLineStringPointer = cmd_buff->_cmd_buffer;

   
    while (*commandLineStringPointer) {
        while (isspace((unsigned char)*commandLineStringPointer)) {
		commandLineStringPointer++; 
	}


        if (*commandLineStringPointer == '\0') {
		break;
	}
        
	if (*commandLineStringPointer == '\"') {
            isQuoted = 1;
            commandLineStringPointer++; 
            cmd_buff->argv[cmd_buff->argc++] = commandLineStringPointer;
            
	    while (*commandLineStringPointer && !(*commandLineStringPointer == '\"' && isQuoted)) {
		    commandLineStringPointer++;
	    }

            if (*commandLineStringPointer == '\"') {
                *commandLineStringPointer = '\0'; 
                commandLineStringPointer++; 
	    }

        } else {
            
	    cmd_buff->argv[cmd_buff->argc++] = commandLineStringPointer;
            
	    while (*commandLineStringPointer && !isspace((unsigned char)*commandLineStringPointer)) {
		    commandLineStringPointer++;
	    }
            
	    if (*commandLineStringPointer) {
                *commandLineStringPointer = '\0'; 
                commandLineStringPointer++;
            }
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
            exit(0);
        case BI_CMD_CD:
            if (cmd->argc > 1) {
                if (chdir(cmd->argv[1]) != 0) {
                    perror("cd failed");
                }
            }
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

int exec_cmd(cmd_buff_t *cmd) {
    if (exec_built_in_cmd(cmd) == BI_EXECUTED) {
	    return OK;
    }

    pid_t pid = fork();
    if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp failed");
        exit(ERR_EXEC_CMD);
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
    } else {
        perror("fork failed");
        return ERR_EXEC_CMD;
    }
    return OK;
}

int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    cmd_buff_t cmd_buff;
    alloc_cmd_buff(&cmd_buff);
    
    while (1) {
        printf(SH_PROMPT);
        
	if (!fgets(input, SH_CMD_MAX, stdin)) {
		break;
	}

        input[strcspn(input, "\n")] = 0;  
        clear_cmd_buff(&cmd_buff);
        
        if (build_cmd_buff(input, &cmd_buff) == OK) {
            exec_cmd(&cmd_buff);
        }
    }
    free_cmd_buff(&cmd_buff);
    return OK;
}
