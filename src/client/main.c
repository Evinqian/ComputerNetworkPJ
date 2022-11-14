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

int client_fd;

int main(int argc, char **argv){

    if (argc != 3) {
        printf("Usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }
    char *hostname = argv[1];
    int port = atoi(argv[2]);

    init();

    printf("Hello client!\n");

    if (Connect(hostname, port) < 0) {
        printf("Connect error\n");
        exit(1);
    }

    char buf[1024];
    int n = write(client_fd, "hello", 1024);
    printf("Wrote %n bytes\n");
    char* res = receive();

    if(!Send(res)){
        printf("send failed!\n");
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

int Connect(char *hostname, int port){
    struct hostent *host;
    struct sockaddr_in server_addr;
    // 根据hostname获取服务器IP地址
    if ((host = gethostbyname(hostname)) == NULL) {
        printf("Can't find IP address of %s\n", hostname);
        return -1;
    }
    for (int i = 0; host->h_addr_list[i]; i++) {
        printf("IP%d: %s\n", i, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    }

    // 设置服务器地址
    memset(&server_addr, 0, sizeof(server_addr));
    memcpy(&server_addr.sin_addr.s_addr, host->h_addr, host->h_length);
    server_addr.sin_port = htons(port);

    printf("server addr: %s\n", inet_ntoa(server_addr.sin_addr));

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