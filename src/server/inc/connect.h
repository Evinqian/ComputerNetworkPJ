#ifndef CNPJ_SERVER_CONNECT_H
#define CNPJ_SERVER_CONNECT_H

#include <stdio.h>

/*启动和初始化服务端*/
void init();

/*在本地端口port开启端口并监听连接*/
void listen(int port);

/*接收一个字符串*/
char* receive();

/*发送一个字符串，返回发送长度*/
int send(char* bytes);

/*结束会话*/
int end_session();

#endif