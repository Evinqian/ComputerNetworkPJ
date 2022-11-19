#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <inc/server/connect.h>
#include <inc/command.h>
#include <inc/io.h>
#define MAX_LEN 1024

int server_fd;
/* 当前目录，~代替根目录 */
char PWD[MAX_LEN + 1] = "~";
/* 根目录 */
char ROOT[MAX_LEN + 1];

int main(int argc, char **argv) {

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    int port = atoi(argv[1]);

    init();
    printf("%s>Hello server!\n", FTP_SERVER);

    Listen(port);
    printf("%s>Listening to %d ...\n", FTP_SERVER, port);

    struct sockaddr_in client_addr;
    int client_len = sizeof(client_addr);

    char buf[MAX_LEN + 1] = { 0 };

    while (1) {
        // 与请求方建立连接
        int conn_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_len);
        if (conn_fd < 0) {
            continue;
        }
        if (fork() == 0) {
            pid_t pid = getpid();
            int r = close(server_fd);
            printf("%s-%d>%s$ ", FTP_SERVER, pid, PWD);
            printf("Server connected to %s\n", inet_ntoa(client_addr.sin_addr));
            // 读取数据
            int n;
            while (1) {
                // 阻塞，直到读取到命令
                n = wait_header(conn_fd, CMD_COMMAND_HEADER, buf, -1);  /* 目前不设最大响应时间 */
                if (n < 0) {
                    // 太长时间未响应或者客户端断开连接，断开连接
                    break;
                } 
                printf("%s-%d>%s$ Received command: %s\n", FTP_SERVER, pid, PWD, buf);
                // 执行命令
                int argc = 0;
                char *argv[MAX_ARGC] = { 0 };
                int r = run_command(conn_fd, buf, &argc, argv);
            }
            printf("%s-%d>%s$ ", FTP_SERVER, pid, PWD);
            printf("Close connection to %s\n", inet_ntoa(client_addr.sin_addr));
            if (close(conn_fd) < 0) {
                printf("Close error\n");
                exit(1);
            }
            exit(0);
        } else {
            if (close(conn_fd) < 0) {
                printf("Close error\n");
                exit(1);
            }
        }
    }
    close(server_fd);
    exit(0);
}

void init() {
    // AF_INET表示正在使用因特网
    // SOCK_STREAM表示套接字是因特网连接的端点
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        printf("Socket error\n");
        exit(1);
    }
    int opt_val = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt_val, sizeof(int)) < 0) {
        printf("Setsockopt error\n");
        exit(1);
    }
    // 获取根目录
    int r = get_pwd(ROOT);
    return;
}

int Listen(int port){
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short) port);
    
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("Port %d has already been used\n", port);
        exit(1);
    }
    // 第二个参数与TCP/IP有关，暂取1024
    if (listen(server_fd, 1024) < 0) {
        printf("Listen error\n");
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

int end_session(){
    printf("end_session() not implemented!\n");
    return 0;
}