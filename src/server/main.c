#include <unistd.h>
#include <inc/server/connect.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h> 

int server_fd;

int main(int argc, char **argv) {

    if (argc != 3 || strcmp(argv[1], "-p") != 0) {
        printf("Usage: %s -p <port>\n", argv[0]);
        exit(1);
    }
    int port =  atoi(argv[2]);

    printf("Hello server!\n");

    init();

    Listen(port);

    struct sockaddr_in client_addr;
    int conn_fd, client_len;

    while (1) {
        conn_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_len);
        if (conn_fd < 0) {
            printf("Accept error\n");
            exit(1);
        }
        struct hostent *hp = gethostbyaddr((const void *) &client_addr.sin_addr.s_addr, 
                    sizeof(client_addr.sin_addr.s_addr), AF_INET);
        char *haddrp = inet_ntoa(client_addr.sin_addr);
        printf("Server connected to %s (%s)\n", hp->h_name, haddrp);
        if (close(conn_fd) < 0) {
            printf("Close error\n");
        }
    }

    char* res = receive();

    if(!Send(res)){
        printf("send failed!\n");
    }

    end_session();
    return 0;
}

void init(){
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
    printf("Listening: %d ...\n", port);
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