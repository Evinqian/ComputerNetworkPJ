#ifndef CNPJ_CLIENT_CONNECT_H
#define CNPJ_CLIENT_CONNECT_H

#include <stdio.h>

/* 客户端文件描述符 */
extern int client_fd;

void init();

/* 连接到host:port */
int Connect(char *host, int port);

/* 接收一个字符串 */
char* receive();

/* 发送一个字符串，返回发送长度 */
int Send(char* bytes);

#endif