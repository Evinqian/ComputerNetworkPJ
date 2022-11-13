#include <inc/client/connect.h>
#include <inc/client/monitor.h>

int main(){
    printf("Hello client!\n");

    connect(15441);

    char* res = receive();

    if(!send(res)){
        printf("send failed!\n");
    }
    
    return 0;
}

void connect(int port){
    printf("connect() not implemented!\n");
    return;
}

char* receive(){
    printf("receive() not implemented!\n");
    return NULL;
}

int send(char* bytes){
    printf("send() not implemented!\n");
    return 0;
}