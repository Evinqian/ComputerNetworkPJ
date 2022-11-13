#include <./inc/connect.h>

int main(){
    printf("Hello server!\n");

    init();

    listen(15441);

    char* res = receive();

    if(!send(res)){
        printf("send failed!\n");
    }

    end_session();
    return 0;
}

void init(){
    printf("init() not implemented!\n");
    return;
}

void listen(int port){
    printf("listen() not implemented!\n");
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

int end_session(){
    printf("end_session() not implemented!\n");
    return 0;
}