#include <sys/stat.h>   
#include <stdlib.h>
#include <string.h>

/* 获取文件大小 */
int get_file_size(char *file_name);

/* 从文件路径中得到文件名 */
void get_file_name(char **file_path);