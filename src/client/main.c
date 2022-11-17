#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <inc/client/connect.h>
#include <inc/command.h>
#include <inc/io.h>
#include <signal.h>

int client_fd;

// 读写时连接断开异常处理
void SIGPIPE_handler (int signum) {
    printf("Server refused your connection\n");
    exit(1);
}

int main(int argc, char **argv){
    if (argc != 3) {
        printf("Usage: %s <host name> <port>\n", argv[0]);
        exit(1);
    }
    char *host_name = argv[1];
    char *port = argv[2];

    // 初始化
    init();
    printf("%s>Hello client!\n", FTP);

    // 建立连接
    if ((client_fd = Connect(host_name, port)) < 0) {
        printf("Failed to connect to %s:%s\n", host_name, port);
        exit(1);
    }
    printf("%s>Successfully connect to %s:%s!\n", FTP, host_name, port);

    // 处理连接断开
    signal(SIGPIPE, SIGPIPE_handler);

    // 等待输入命令
    char line[MAX_LINE + 1];
    buf_io_t buf_io;
    buf_io_init(&buf_io, client_fd);

    while (1) {
        printf("%s>", FTP);
        // 等待用户输入命令
        if (fgets(line, MAX_LINE, stdin) == NULL && ferror(stdin)) {
            break;
        }

        // 执行命令
        int argc = 0;
	    char *argv[MAX_ARGC] = { 0 };
        int r = run_command(line, &argc, argv);
        extern char cmd_error_msg[];
        extern char cmd_msg[];
        
        // 根据结果输出到控制台
        switch (r) {
            case CMD_WRONG_USAGE:
                printf("Usage: ");
                print_command_usage(argv[0]);
                putchar('\n');
                break;
            case CMD_ERROR:
                printf("%s: %s\n", argv[0], cmd_error_msg);
                break;
            case CMD_UNKNOWN:
                printf("Unknown command: %s\n", argv[0]);
                break;
            case CMD_TOO_MANT_ARGS:
                printf("Too many argments: %d (max %d)\n", argc, MAX_ARGC);
                break;
            case CMD_QUIT:
                if (close(client_fd) < 0) {
                    printf("Close error\n");
                    exit(1);
                }
                exit(0);
            default:
                // 正确执行，将命令发给服务端，由服务端执行
                if (strcmp(cmd_msg, "") != 0) {
                    printf("%s: %s\n", argv[0], cmd_msg);
                }
                break;
        }
    }
}

void init(){
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) {
        printf("Socket error\n");
        exit(1);
    }
}

int Connect(char *host_name, char *port){
    struct addrinfo hints, *addr_list, *addr;
    int client_fd;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_NUMERICSERV; 
    hints.ai_flags |= AI_ADDRCONFIG;  /* 推荐 */
    
    // 获取服务器地址列表
    struct sockaddr_in server_addr;
    if (getaddrinfo(host_name, port, &hints, &addr_list) != 0) {
        printf("Getaddrinfo error: %s:%s\n", host_name, port);
        return -1;
    }

    // 遍历所有地址，直到找到能连接的
    for (addr = addr_list; addr; addr = addr->ai_next) {
        // 尝试创建套接字
        if ((client_fd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
            continue;
        }

        // 尝试连接
        if (connect(client_fd, addr->ai_addr, addr->ai_addrlen) != -1) {
            break; 
        }
        if (close(client_fd) < 0) {
            printf("Close error\n");
            return -1;
        } 
    } 
    freeaddrinfo(addr_list);
    if (!addr) {
        // 没有任何成功
        return -1;
    } else {   
        return client_fd;
    }

    if (connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        return -1;
    }
    return 0;
}

char* receive(){
    printf("receive() not implemented!\n");
    return NULL;
}

int Send(char* bytes){
    printf("send() not implemented!\n");
    return 0;
}