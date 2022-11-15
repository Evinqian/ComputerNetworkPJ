#ifndef CNPJ_CLIENT_MONITOR_H
#define CNPJ_CLIENT_MONITOR_H

#include <inc/client/connect.h>
#define CMD_UNKNOWN 1
#define CMD_TOO_MANT_ARGS 2
#define CMD_QUIT 3
#define MAX_ARGC 16

/* 根据命令行输入并匹配命令 */
int run(char *buf, int* argc, char** argv);

int ls(int argc, char** argv);

int pwd(int argc, char** argv);

int cd(int argc, char** argv);

int mkdir(int argc, char** argv);

int get(int argc, char** argv);

int put(int argc, char** argv);

int del(int argc, char** argv);

int quit(int argc, char** argv);

#endif