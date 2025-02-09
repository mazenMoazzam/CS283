#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "dshlib.h"

/*
 *  build_cmd_list
 *    cmd_line:     the command line from the user
 *    clist *:      pointer to clist structure to be populated
 *
 *  This function builds the command_list_t structure passed by the caller
 *  It does this by first splitting the cmd_line into commands by spltting
 *  the string based on any pipe characters '|'.  It then traverses each
 *  command.  For each command (a substring of cmd_line), it then parses
 *  that command by taking the first token as the executable name, and
 *  then the remaining tokens as the arguments.
 *
 *  NOTE your implementation should be able to handle properly removing
 *  leading and trailing spaces!
 *
 *  errors returned:
 *
 *    OK:                      No Error
 *    ERR_TOO_MANY_COMMANDS:   There is a limit of CMD_MAX (see dshlib.h)
 *                             commands.
 *    ERR_CMD_OR_ARGS_TOO_BIG: One of the commands provided by the user
 *                             was larger than allowed, either the
 *                             executable name, or the arg string.
 *
 *  Standard Library Functions You Might Want To Consider Using
 *      memset(), strcmp(), strcpy(), strtok(), strlen(), strchr()
 */

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
        char *spacePosition = strchr(command, ' ');
        if (spacePosition != NULL) {
            *spacePosition = '\0';  
            strcpy(clist->commands[commandCount].exe, command);
            strcpy(clist->commands[commandCount].args, spacePosition + 1);
        } else {     
            strcpy(clist->commands[commandCount].exe, command);
            clist->commands[commandCount].args[0] = '\0'; 
        }

       
        commandCount++;       
        command = strtok(NULL, PIPE_STRING);
    }
    clist->num = commandCount;
    return OK;
}

