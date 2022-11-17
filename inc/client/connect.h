#ifndef CNPJ_CLIENT_CONNECT_H
#define CNPJ_CLIENT_CONNECT_H
#define FTP "ftp"

void init();

/* 连接到host:port */
int Connect(char *IP, char *port);

/* 接收一个字符串 */
char* receive();

/* 发送一个字符串，返回发送长度 */
int Send(char* bytes);

#endif