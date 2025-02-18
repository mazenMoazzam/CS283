#include "dshlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int alloc_cmd_buff(cmd_buff_t *cmd_buff) {
    cmd_buff->_cmd_buffer = malloc(SH_CMD_MAX);
    if (!cmd_buff->_cmd_buffer) {
	    return ERR_MEMORY;
    }
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
    if (!cmd_line || strlen(cmd_line) > SH_CMD_MAX)
        return ERR_CMD_OR_ARGS_TOO_BIG;

    cmd_buff->argc = 0;
    cmd_buff->_cmd_buffer = strdup(cmd_line);
    if (!cmd_buff->_cmd_buffer)
        return ERR_MEMORY;

    char *buffer = cmd_buff->_cmd_buffer;
    char *arg;
    int in_quotes = 0;

    while (*buffer) {
        while (*buffer == SPACE_CHAR) {
            buffer++;  // Skip spaces
        }

        if (*buffer == '\0') {
            break;
        }

       
        if (*buffer == '"') {  // Start of quoted argument
            in_quotes = 1;
            buffer++;  // Skip the opening quote
            arg = buffer;  // Mark the start of the argument
            while (*buffer && (*buffer != '"' || (buffer > arg && *(buffer - 1) == '\\'))) {
                buffer++;  // Continue until the closing quote or a backslash is found before a quote
            }
            if (*buffer == '"') {  // End of quoted argument
                *buffer = '\0';  // Null-terminate the argument
                buffer++;  // Skip over the closing quote
            }
        } else {  // Regular argument (no quotes)
            arg = buffer;
            while (*buffer && *buffer != SPACE_CHAR) {
                buffer++;  // Continue until space or end of line
            }
            if (*buffer != '\0') {
                *buffer = '\0';  // Null-terminate the argument
                buffer++;  // Skip over the space
            }
        }

        cmd_buff->argv[cmd_buff->argc++] = arg;  // Save the argument
    }

    return BI_EXECUTED; 
}








Built_In_Cmds match_command(const char *input) {
    if (strcmp(input, EXIT_CMD) == 0) return BI_CMD_EXIT;
    if (strcmp(input, "cd") == 0) return BI_CMD_CD;
    if (strcmp(input, "dragon") == 0) return BI_CMD_DRAGON;
    return BI_NOT_BI;
}


Built_In_Cmds exec_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds cmd_type = match_command(cmd->argv[0]);
    switch (cmd_type) {
        case BI_CMD_EXIT:
            exit(OK_EXIT);
        case BI_CMD_CD:
            if (cmd->argc < 2) {
                return ERR_CMD_ARGS_BAD;
            }
            if (chdir(cmd->argv[1]) != 0) {
                perror("cd failed");
            } else {
            }
            return BI_EXECUTED;
        case BI_CMD_DRAGON:
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}


int exec_cmd(cmd_buff_t *cmd) {
    pid_t pid = fork();
    if (pid < 0) return ERR_EXEC_CMD;
    else if (pid == 0) {
        execvp(cmd->argv[0], cmd->argv);
        perror("execvp failed");
        exit(ERR_EXEC_CMD);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : ERR_EXEC_CMD;
    }
}


int exec_local_cmd_loop() {
    char input[SH_CMD_MAX];
    cmd_buff_t cmd;
    
    if (alloc_cmd_buff(&cmd) != OK) return ERR_MEMORY;
    
    while (1) {
        printf(SH_PROMPT);
        if (!fgets(input, SH_CMD_MAX, stdin)) break;
        input[strcspn(input, "\n")] = 0; 
        
        if (build_cmd_buff(input, &cmd) == WARN_NO_CMDS) continue;
        if (exec_built_in_cmd(&cmd) == BI_NOT_BI) exec_cmd(&cmd);
    }
    
    free_cmd_buff(&cmd);
    return OK;
}

