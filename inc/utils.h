#include <sys/stat.h>   
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>    
#include <sys/stat.h>   
#include <sys/socket.h>    
#include <signal.h>    
#include <fcntl.h>
#include <time.h>

/* 获取文件大小 */
int get_file_size(char *file_name);

/* 从文件路径中得到文件名 */
void get_file_name(char **file_path);