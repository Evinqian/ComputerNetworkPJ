#include <unistd.h>
#include <inc/server/connect.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <inc/client/connect.h>
#include <inc/client/monitor.h>
// #include "inc/csapp.h"
#define MAXLEN 1024

int client_fd;

int main(int argc, char **argv){

    if (argc != 3) {
        printf("Usage: %s <host name> <port>\n", argv[0]);
        exit(1);
    }
    char *host_name = argv[1];
    char *port = argv[2];

    init();
    printf("ftp>Hello client!\n");

    if ((client_fd = Connect(host_name, port)) < 0) {
        printf("Failed to connect to %s:%d\n", host_name, port);
        exit(1);
    }
    printf("ftp>Connect successfully!\n");

    char buf[MAXLEN];

    while (1) {
        printf("ftp>");
        int n = scanf("%s", buf);
        printf("ftp>Input %d bytes: %s\n", n, buf);

        n = write(client_fd, buf, 1);
        if (n < 0) {
            printf("Write error\n");
            exit(1);
        }
        printf("ftp>Wrote %d bytes: %s\n", n, buf);
    }

    if (close(client_fd) < 0) {
        printf("Close error\n");
        exit(1);
    }
    exit(0);
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