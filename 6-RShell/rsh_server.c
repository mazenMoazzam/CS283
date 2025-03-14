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

/*
 * start_server(ifaces, port, is_threaded)
 *      ifaces:  a string in ip address format, indicating the interface
 *              where the server will bind.  In almost all cases it will
 *              be the default "0.0.0.0" which binds to all interfaces.
 *              note the constant RDSH_DEF_SVR_INTFACE in rshlib.h
 * 
 *      port:   The port the server will use.  Note the constant 
 *              RDSH_DEF_PORT which is 1234 in rshlib.h.  If you are using
 *              tux you may need to change this to your own default, or even
 *              better use the command line override -s implemented in dsh_cli.c
 *              For example ./dsh -s 0.0.0.0:5678 where 5678 is the new port  
 * 
 *      is_threaded:  Used for extra credit to indicate the server should implement
 *                   per thread connections for clients  
 * 
 *      This function basically runs the server by: 
 *          1. Booting up the server
 *          2. Processing client requests until the client requests the
 *             server to stop by running the `stop-server` command
 *          3. Stopping the server. 
 */
int start_server(char *ifaces, int port, int is_threaded) {
    int svr_socket;
    int rc;

    // Suppress unused parameter warning
    (void)is_threaded;

    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0) {
        int err_code = svr_socket;  // server socket will carry error code
        return err_code;
    }

    rc = process_cli_requests(svr_socket);

    stop_server(svr_socket);

    return rc;
}

/*
 * stop_server(svr_socket)
 *      svr_socket: The socket that was created in the boot_server()
 *                  function. 
 * 
 *      This function simply returns the value of close() when closing
 *      the socket.  
 */
int stop_server(int svr_socket) {
    return close(svr_socket);
}

/*
 * boot_server(ifaces, port)
 *      ifaces & port:  see start_server for description.  They are passed
 *                      as is to this function.   
 * 
 *      This function "boots" the rsh server.  It is responsible for all
 *      socket operations prior to accepting client connections.  
 */
int boot_server(char *ifaces, int port) {
    int svr_socket;
    int ret;
    struct sockaddr_in addr;

    // Create the server socket
    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0) {
        perror("socket");
        return ERR_RDSH_COMMUNICATION;
    }

    // Allow reuse of the port (useful during development)
    int enable = 1;
    if (setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    // Set up the server address
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ifaces, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    // Bind the socket to the address and port
    ret = bind(svr_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    // Start listening for client connections
    ret = listen(svr_socket, 20);
    if (ret < 0) {
        perror("listen");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    return svr_socket;
}

/*
 * process_cli_requests(svr_socket)
 *      svr_socket:  The server socket that was obtained from boot_server()
 *   
 *  This function handles managing client connections.
 */
int process_cli_requests(int svr_socket) {
    int cli_socket;
    struct sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    int rc = OK;

    while (1) {
        // Accept a client connection
        cli_socket = accept(svr_socket, (struct sockaddr *)&cli_addr, &cli_len);
        if (cli_socket < 0) {
            perror("accept");
            return ERR_RDSH_COMMUNICATION;
        }

        // Handle client requests
        rc = exec_client_requests(cli_socket);
        if (rc == OK_EXIT) {
            // Client requested server to stop
            close(cli_socket);
            break;
        }

        // Close the client socket after handling requests
        close(cli_socket);
    }

    return rc;
}

/*
 * exec_client_requests(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 *   
 *  This function handles accepting remote client commands.
 */
int exec_client_requests(int cli_socket) {
    int io_size;
    command_list_t cmd_list;
    int rc;
    char *io_buff;

    // Allocate a buffer for receiving commands
    io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!io_buff) {
        return ERR_RDSH_SERVER;
    }

    while (1) {
        // Receive the command from the client
        io_size = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);
        if (io_size < 0) {
            perror("recv");
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        } else if (io_size == 0) {
            // Client disconnected
            free(io_buff);
            return OK;
        }

        // Null-terminate the received command
        io_buff[io_size] = '\0';

        // Parse the command into a command list
        rc = build_cmd_list(io_buff, &cmd_list);
        if (rc < 0) {
            // Handle parsing errors
            send_message_string(cli_socket, CMD_ERR_RDSH_EXEC);
            send_message_eof(cli_socket);
            continue;
        }

        // Handle built-in commands
        Built_In_Cmds bi_cmd = rsh_built_in_cmd(&cmd_list.commands[0]);
        if (bi_cmd == BI_CMD_EXIT) {
            // Client requested to exit
            free(io_buff);
            return OK;
        } else if (bi_cmd == BI_CMD_STOP_SVR) {
            // Client requested server to stop
            free(io_buff);
            return OK_EXIT;
        } else if (bi_cmd == BI_EXECUTED) {
            // Built-in command executed (e.g., cd)
            send_message_eof(cli_socket);
            continue;
        }

        // Execute the command pipeline
        rc = rsh_execute_pipeline(cli_socket, &cmd_list);
        if (rc < 0) {
            // Handle execution errors
            send_message_string(cli_socket, CMD_ERR_RDSH_EXEC);
        }

        // Send EOF to indicate end of response
        send_message_eof(cli_socket);
    }

    free(io_buff);
    return OK;
}

/*
 * rsh_execute_pipeline(int cli_sock, command_list_t *clist)
 *      cli_sock:    The server-side socket that is connected to the client
 *      clist:       The command_list_t structure that we implemented in
 *                   the last shell. 
 *   
 *  This function executes the command pipeline.
 */
int rsh_execute_pipeline(int cli_sock, command_list_t *clist) {
    int pipes[clist->num - 1][2];  // Array of pipes
    pid_t pids[clist->num];
    int pids_st[clist->num];       // Array to store process statuses
    int exit_code;

    // Create pipes for the pipeline
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_RDSH_COMMUNICATION;
        }
    }

    // Fork and execute each command in the pipeline
    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            return ERR_RDSH_COMMUNICATION;
        } else if (pids[i] == 0) {
            // Child process
            if (i > 0) {
                // Redirect stdin from the previous pipe
                dup2(pipes[i - 1][0], STDIN_FILENO);
            } else {
                // First process: redirect stdin from the client socket
                dup2(cli_sock, STDIN_FILENO);
            }

            if (i < clist->num - 1) {
                // Redirect stdout to the next pipe
                dup2(pipes[i][1], STDOUT_FILENO);
            } else {
                // Last process: redirect stdout to the client socket
                dup2(cli_sock, STDOUT_FILENO);
            }

            // Close all pipe ends
            for (int j = 0; j < clist->num - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute the command
            execvp(clist->commands[i].argv[0], clist->commands[i].argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    // Parent process: close all pipe ends
    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &pids_st[i], 0);
    }

    // Get the exit code of the last process in the pipeline
    exit_code = WEXITSTATUS(pids_st[clist->num - 1]);
    return exit_code;
}

/*
 * send_message_eof(cli_socket)
 *      cli_socket:  The server-side socket that is connected to the client
 * 
 *  Sends the EOF character to the client.
 */
int send_message_eof(int cli_socket) {
    int send_len = (int)sizeof(RDSH_EOF_CHAR);
    int sent_len;
    sent_len = send(cli_socket, &RDSH_EOF_CHAR, send_len, 0);

    if (sent_len != send_len) {
        return ERR_RDSH_COMMUNICATION;
    }
    return OK;
}

/*
 * send_message_string(cli_socket, char *buff)
 *      cli_socket:  The server-side socket that is connected to the client
 *      buff:        A C string (aka null terminated) of a message we want
 *                   to send to the client. 
 *   
 *  Sends a message to the client.
 */
int send_message_string(int cli_socket, char *buff) {
    int sent_len;

    // Send the message
    sent_len = send(cli_socket, buff, strlen(buff), 0);
    if (sent_len < 0) {
        perror("send");
        return ERR_RDSH_COMMUNICATION;
    }

    // Send the EOF character
    return send_message_eof(cli_socket);
}

Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd) {
    if (cmd->argc == 0) {
        return BI_NOT_BI;
    }

    if (strcmp(cmd->argv[0], "cd") == 0) {
        // Handle cd command
        if (cmd->argc < 2) {
            // No directory provided, default to home directory
            chdir(getenv("HOME"));
        } else {
            if (chdir(cmd->argv[1]) < 0) {
                perror("cd");
            }
        }
        return BI_EXECUTED;
    } else if (strcmp(cmd->argv[0], "exit") == 0) {
        // Handle exit command
        return BI_CMD_EXIT;
    } else if (strcmp(cmd->argv[0], "stop-server") == 0) {
        // Handle stop-server command
        return BI_CMD_STOP_SVR;
    }

    // Command is not built-in
    return BI_NOT_BI;
}
