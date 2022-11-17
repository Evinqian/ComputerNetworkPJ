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
int conn_fd;

static char *to_upper(char *s) {
    char *t = s;
    while (*t) {
        if (*t >= 'a' && *t <= 'z') {
            *t -= 32;
        }
        t++;
    }
    return s;
}

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
        conn_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_len);
        printf("%s>", FTP_SERVER);
        if (conn_fd < 0) {
            printf("Accept error\n");
            exit(1);
        }

        printf("Server connected to %s\n", inet_ntoa(client_addr.sin_addr));

        // 读取数据
        buf_io_t buf_io;
        buf_io_init(&buf_io, conn_fd);
        int n;
        char *command = NULL;

        // while((n = buf_read_line(&buf_io, buf, MAX_LEN)) != 0) {
        while (1) {
            // 阻塞，直到读取到命令
            wait_header(conn_fd, CMD_COMMAND_HEADER, buf);
            printf("%s>Received command: %s", FTP_SERVER, buf);

            // 执行命令
            int argc = 0;
	        char *argv[MAX_ARGC] = { 0 };
            int r = run_command(buf, &argc, argv);
        }
        printf("Close connection to %s\n", inet_ntoa(client_addr.sin_addr));
        if (close(conn_fd) < 0) {
            printf("Close error\n");
        }
    }
    return 0;
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
    return;
}

int Listen(int port){
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons((unsigned short) port);
    
    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        printf("Bind error\n");
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