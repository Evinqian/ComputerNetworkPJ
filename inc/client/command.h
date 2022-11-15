#ifndef CNPJ_CLIENT_MONITOR_H
#define CNPJ_CLIENT_MONITOR_H

#define CMD_UNKNOWN 1
#define CMD_TOO_MANT_ARGS 2
#define CMD_QUIT 3
#define CMD_WRONG_USAGE 4
#define CMD_ERROR 5
#define MAX_ARGC 16
#define MAX_LINE 1024
#define MAX_LEN 1024

char cmd_msg[MAX_LEN];
char cmd_error_msg[MAX_LEN];

/* 根据命令行输入并匹配命令 */
int run(char *buf, int* argc, char** argv);

int print_usage(char *name);

/*  
 *  argc和argv是直接解析得到的参数
 *  - 如果参数不正确，返回CMD_WRONG_USAGE
 *  - 如果执行错误，返回CMD_ERROR，并将错误信息放在cmd_error_msg中
 *  - 如果执行正常，返回0，并将信息放在cmd_msg中
 */
int ls(int argc, char** argv);

int pwd(int argc, char** argv);

int cd(int argc, char** argv);

int Mkdir(int argc, char** argv);

int get(int argc, char** argv);

int put(int argc, char** argv);

int Delete(int argc, char** argv);

int quit(int argc, char** argv);

#endif