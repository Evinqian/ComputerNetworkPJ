#ifndef CNPJ_CLIENT_CONNECT_H
#define CNPJ_CLIENT_CONNECT_H

#include <stdio.h>

/*连接到在本地端口port*/
void connect(int port);

/*接收一个字符串*/
char* receive();

/*发送一个字符串，返回发送长度*/
int send(char* bytes);

#endif