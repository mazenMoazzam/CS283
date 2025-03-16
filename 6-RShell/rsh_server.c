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
    (void)is_threaded;

    svr_socket = boot_server(ifaces, port);
    if (svr_socket < 0) {
        int err_code = svr_socket;  
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
    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_socket < 0) {
        perror("socket");
        return ERR_RDSH_COMMUNICATION;
    }

   
    int enable = 1;
    if (setsockopt(svr_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ifaces, &addr.sin_addr) <= 0) {
        perror("inet_pton");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
    ret = bind(svr_socket, (struct sockaddr *)&addr, sizeof(addr));
    if (ret < 0) {
        perror("bind");
        close(svr_socket);
        return ERR_RDSH_COMMUNICATION;
    }
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

        cli_socket = accept(svr_socket, (struct sockaddr *)&cli_addr, &cli_len);
        if (cli_socket < 0) {
            perror("accept");
            return ERR_RDSH_COMMUNICATION;
        }
        rc = exec_client_requests(cli_socket);
        if (rc == OK_EXIT) {
            close(cli_socket);
            break;
        }
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
    io_buff = malloc(RDSH_COMM_BUFF_SZ);
    if (!io_buff) {
        return ERR_RDSH_SERVER;
    }

    while (1) {
        io_size = recv(cli_socket, io_buff, RDSH_COMM_BUFF_SZ, 0);
        if (io_size < 0) {
            perror("recv");
            free(io_buff);
            return ERR_RDSH_COMMUNICATION;
        } else if (io_size == 0) {
            free(io_buff);
            return OK;
        }
        io_buff[io_size] = '\0';
        rc = build_cmd_list(io_buff, &cmd_list);
        if (rc < 0) {
            send_message_string(cli_socket, CMD_ERR_RDSH_EXEC);
            send_message_eof(cli_socket);
            continue;
        }

        
        Built_In_Cmds bi_cmd = rsh_built_in_cmd(&cmd_list.commands[0]);
        if (bi_cmd == BI_CMD_EXIT) {
            free(io_buff);
            return OK;
        } else if (bi_cmd == BI_CMD_STOP_SVR) {
            
            free(io_buff);
            return OK_EXIT;
        } else if (bi_cmd == BI_EXECUTED) {
          
            send_message_eof(cli_socket);
            continue;
        }

        
        rc = rsh_execute_pipeline(cli_socket, &cmd_list);
        if (rc < 0) {
           
            send_message_string(cli_socket, CMD_ERR_RDSH_EXEC);
        }

        
        rc = send_message_eof(cli_socket);
        if (rc != OK) {
            printf(CMD_ERR_RDSH_COMM);
            free(io_buff);
            close(cli_socket);
            return ERR_RDSH_COMMUNICATION;
        }
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
    int pipes[clist->num - 1][2];  
    pid_t pids[clist->num];
    int pids_st[clist->num];       
    int exit_code;
    for (int i = 0; i < clist->num - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return ERR_RDSH_COMMUNICATION;
        }
    }

    for (int i = 0; i < clist->num; i++) {
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
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
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < clist->num - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

   
    for (int i = 0; i < clist->num; i++) {
        waitpid(pids[i], &pids_st[i], 0);
   
    }
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
    sent_len = send(cli_socket, buff, strlen(buff), 0);
    if (sent_len < 0) {
        perror("send");
        return ERR_RDSH_COMMUNICATION;
    }
    return send_message_eof(cli_socket);
}

/*
 * rsh_built_in_cmd(cmd_buff_t *cmd)
 *      cmd:  The cmd_buff_t of the command, remember, this is the 
 *            parsed version of the command
 *   
 *  This function checks if the command is built-in or not. If it is,
 *  it handles the execution of the built-in command.
 */
Built_In_Cmds rsh_built_in_cmd(cmd_buff_t *cmd)
{
    Built_In_Cmds ctype = BI_NOT_BI;
    ctype = rsh_match_command(cmd->argv[0]);

    switch (ctype)
    {
    // case BI_CMD_DRAGON:
    //     print_dragon();
    //     return BI_EXECUTED;
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
