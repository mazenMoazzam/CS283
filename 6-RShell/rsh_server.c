#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>

#include "dshlib.h"
#include "rshlib.h"

int start_server(char *ifaces, int port, int is_threaded) {
    int serverSocket;
    int returnCode;
    (void)is_threaded;

    serverSocket = boot_server(ifaces, port);
    
    if (serverSocket < 0) {
        int errorCode = serverSocket;  
        return errorCode;
    }

    returnCode = process_cli_requests(serverSocket);
    stop_server(serverSocket);
    return returnCode;
}

int stop_server(int svr_socket) {
    return close(svr_socket);
}

int boot_server(char *ifaces, int port) {
    int serverSocket;
    int returnStatusCode;
    struct sockaddr_in addr;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        return ERR_RDSH_COMMUNICATION;
    }

    int enable = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        close(serverSocket);
        return ERR_RDSH_COMMUNICATION;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ifaces, &addr.sin_addr) <= 0) {
        close(serverSocket);
        return ERR_RDSH_COMMUNICATION;
    }

    returnStatusCode = bind(serverSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (returnStatusCode < 0) {
        close(serverSocket);
        return ERR_RDSH_COMMUNICATION;
    }
    
    returnStatusCode = listen(serverSocket, 20);
    if (returnStatusCode < 0) {
        close(serverSocket);
        return ERR_RDSH_COMMUNICATION;
    }

    return serverSocket;
}

int process_cli_requests(int svr_socket) {
    int clientSocket;
    struct sockaddr_in cliAddr;
    socklen_t cliLen = sizeof(cliAddr);
    int statusReturnCode = OK;

    while (1) {

	clientSocket = accept(svr_socket, (struct sockaddr *)&cliAddr, &cliLen);
        if (clientSocket < 0) {
            return ERR_RDSH_COMMUNICATION;
        }
        statusReturnCode = exec_client_requests(clientSocket);
        if (statusReturnCode == OK_EXIT) {
            close(clientSocket);
            break;
        }
        close(clientSocket);
    }

    return statusReturnCode;
}






int exec_client_requests(int cli_socket) {
    int ioSize;
    command_list_t cmdList;
    int returnCode;
    char *ioBuffSize;
    ioBuffSize = malloc(RDSH_COMM_BUFF_SZ);
    
    if (!ioBuffSize) {
        return ERR_RDSH_SERVER;
    }

    while (1) {
	ioSize = recv(cli_socket, ioBuffSize, RDSH_COMM_BUFF_SZ, 0);
        if (ioSize < 0) {
            free(ioBuffSize);
            return ERR_RDSH_COMMUNICATION;
        } else if (ioSize == 0) {
            free(ioBuffSize);
            return OK;
        }
        ioBuffSize[ioSize] = '\0';
        returnCode = build_cmd_list(ioBuffSize, &cmdList);
        if (returnCode < 0) {
            send_message_string(cli_socket, CMD_ERR_RDSH_EXEC);
            send_message_eof(cli_socket);
            continue;
        }

        Built_In_Cmds biCmd = rsh_built_in_cmd(&cmdList.commands[0]);
        if (biCmd == BI_CMD_EXIT) {
            free(ioBuffSize);
            return OK;
        } else if (biCmd == BI_CMD_STOP_SVR) {
            free(ioBuffSize);
            return OK_EXIT;
        } else if (biCmd == BI_EXECUTED) {
            send_message_eof(cli_socket);
            continue;
        }

        returnCode = rsh_execute_pipeline(cli_socket, &cmdList);
        if (returnCode < 0) {
            send_message_string(cli_socket, CMD_ERR_RDSH_EXEC);
        }

        returnCode = send_message_eof(cli_socket);
        if (returnCode != OK) {
            printf(CMD_ERR_RDSH_COMM);
            free(ioBuffSize);
            close(cli_socket);
            return ERR_RDSH_COMMUNICATION;
        }
    }

    free(ioBuffSize);
    return OK;
}

int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {
    int pipes[clist->num - 1][2];  
    pid_t pids[clist->num];
    int pidsSt[clist->num];       
    int exitCode;


    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            return ERR_RDSH_COMMUNICATION;
        }
    }

    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            return ERR_RDSH_COMMUNICATION;
        } else if (pids[i] == 0) {
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            } else {
                dup2(cli_sock, STDIN_FILENO);
            }

            if (i < clist->num - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            } else {
                if (!clist->commands[i].outputFile) {
                    dup2(cli_sock, STDOUT_FILENO);
                    dup2(cli_sock, STDERR_FILENO);
                }
            }

            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &pidsSt[i], 0);
    }
    exitCode = WEXITSTATUS(pidsSt[clist->num - 1]);
    return exitCode;
}

int send_message_eof(int cli_socket) {
    int sendLen = (int)sizeof(RDSH_EOF_CHAR);
    int sentLen;
    sentLen = send(cli_socket, &RDSH_EOF_CHAR, sendLen, 0);

    if (sentLen != sendLen) {
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}

int send_message_string(int cli_socket, char *buff) {
    int sentLen;
    sentLen = send(cli_socket, buff, strlen(buff), 0);
    if (sentLen < 0) {
        return ERR_RDSH_COMMUNICATION;
    }
    return send_message_eof(cli_socket);
}

Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd) {
    Built_In_Cmds ctype = BI_NOT_BI;
    ctype = rsh_match_command(cmd->argv[0]);

    switch (ctype) {
        case BI_CMD_EXIT:
            return BI_CMD_EXIT;
        case BI_CMD_STOP_SVR:
            return BI_CMD_STOP_SVR;
        case BI_CMD_RC:
            return BI_CMD_RC;
        case BI_CMD_CD:
            chdir(cmd->argv[1]);
            return BI_EXECUTED;
        default:
            return BI_NOT_BI;
    }
}

Built_In_Cmds rsh_match_command(const char *cmd) {
    if (cmd == NULL) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd, "cd") == 0) {
        return BI_CMD_CD;
    } else if (strcmp(cmd, "exit") == 0) {
        return BI_CMD_EXIT;
    } else if (strcmp(cmd, "stop-server") == 0) {
        return BI_CMD_STOP_SVR;
    } else if (strcmp(cmd, "rc") == 0) {
        return BI_CMD_RC;
    } else {
        return BI_NOT_BI;
    }
}
