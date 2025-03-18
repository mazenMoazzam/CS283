#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <fcntl.h>

#include "dshlib.h"
#include "rshlib.h"

int exec_remote_cmd_loop(char *address, int port) {
    char *cmdBuff = NULL;
    char *rspBuff = NULL;
    int clientSocket;
    ssize_t ioSize;
    int isEOF;

    cmdBuff = malloc(RDSH_COMM_BUFF_SZ);
    rspBuff = malloc(RDSH_COMM_BUFF_SZ);
    if (!cmdBuff || !rspBuff) {
        return client_cleanup(-1, cmdBuff, rspBuff, ERR_MEMORY);
    }

    clientSocket = start_client(address, port);
    if (clientSocket < 0) {
        return client_cleanup(clientSocket, cmdBuff, rspBuff, ERR_RDSH_CLIENT);
    }
    while (1) {
        printf(SH_PROMPT);
        fflush(stdout);

        if (!fgets(cmdBuff, RDSH_COMM_BUFF_SZ, stdin)) {
            return client_cleanup(clientSocket, cmdBuff, rspBuff, ERR_RDSH_COMMUNICATION);
        }

        cmdBuff[strcspn(cmdBuff, "\n")] = '\0';

        ioSize = send(clientSocket, cmdBuff, strlen(cmdBuff) + 1, 0);
        if (ioSize < 0) {
            return client_cleanup(clientSocket, cmdBuff, rspBuff, ERR_RDSH_COMMUNICATION);
        }

        if (strcmp(cmdBuff, "exit") == 0) {
            break;
        }

        while (1) {
            ioSize = recv(clientSocket, rspBuff, RDSH_COMM_BUFF_SZ, 0);
            if (ioSize < 0) {
                return client_cleanup(clientSocket, cmdBuff, rspBuff, ERR_RDSH_COMMUNICATION);
            } else if (ioSize == 0) {
                printf(RCMD_MSG_CLIENT_EXITED);
                return client_cleanup(clientSocket, cmdBuff, rspBuff, OK);
            }

            isEOF = (rspBuff[ioSize - 1] == RDSH_EOF_CHAR);
            if (isEOF) {
                rspBuff[ioSize - 1] = '\0';
            }

            printf("%.*s", (int)ioSize, rspBuff);

            if (isEOF) {
                break;
            }
        }
    }

    return client_cleanup(clientSocket, cmdBuff, rspBuff, OK);
}

int start_client(char *server_ip, int port) {
    struct sockaddr_in addr;
    int clientSocket;
    int returnCode;
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        return ERR_RDSH_CLIENT;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &addr.sin_addr) <= 0) {
        close(clientSocket);
        return ERR_RDSH_CLIENT;
    }
    returnCode = connect(clientSocket, (struct sockaddr *)&addr, sizeof(addr));
    if (returnCode < 0) {
        close(clientSocket);
        return ERR_RDSH_CLIENT;
    }

    return clientSocket;
}

int client_cleanup(int cli_socket, char *cmd_buff, char *rsp_buff, int rc){
    if (cli_socket > 0) {
        close(cli_socket);
    }

    free(cmd_buff);
    free(rsp_buff);
    return rc;
}
