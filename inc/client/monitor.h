#ifndef CNPJ_CLIENT_MONITOR_H
#define CNPJ_CLIENT_MONITOR_H

#include <inc/client/connect.h>

/*获取命令行输入并匹配命令*/
int get_command();

char* ls();

char* pwd();

char* cd(char* dir);

int mkdir(char* dir_name);

int get(char* filename);

int put(char* filename);

int del(char* filename);

#endif