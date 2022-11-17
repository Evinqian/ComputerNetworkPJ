#ifndef CNPJ_CLIENT_MONITOR_H
#define CNPJ_CLIENT_MONITOR_H

/* 命令返回值 */
#define CMD_UNKNOWN 1
#define CMD_TOO_MANT_ARGS 2
#define CMD_QUIT 3
#define CMD_WRONG_USAGE 4
#define CMD_ERROR 5
/* 数据头部 */
#define CMD_COMMAND_HEADER "CMD"
#define CMD_SIZE_HEADER "SIZE"
/* 最大限制 */
#define MAX_ARGC 16
#define MAX_LINE 1024
#define MAX_LEN 1024

/* 增加头部并返回 */
void add_header(char* buf, const char* header);

/* 返回去除头部的第一个地址如果具有头部header，否则返回NULL */
char *check_header(char *command, const char *header);

/* 阻塞，直到从fd读到具有header头部的数据
 * 若data不为NULL，则将数据存入data
 * 读取到'\n'开始匹配头部,如果不匹配将会丢弃
 */
void wait_header(int fd, const char *header, char *data);

/* 根据命令行输入并匹配命令 */
int run_command(char *buf, int* argc, char** argv);

/* 进度条，当前进度: 0 <= process <= 1 */
char *process_bar(double process, int length, char *buf);

/* 将命令用法打印到标准输出 */
int print_command_usage(char *name);

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