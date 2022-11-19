#include <inc/command.h>
#include <inc/server/connect.h>
#include <inc/io.h>
#include <inc/utils.h>

/* 服务端描述符 */
extern int server_fd;
/* 当前目录 */
extern char PWD[];
/* 根目录 */
extern char ROOT[];

int get_pwd(char *buf) {
	char *argv[] = { "pwd" }; 
    int r = exec(1, argv, buf);
	if (r < 0) {
		return r;
	}
    *strchr(buf, '\n') = 0;
	return 0;
}

/* 判断pa是否是dir的父级目录，返回相对父目录的路径 */
static char *is_parent(char *dir, char* pa) {
	int n = strlen(pa);
	if (strlen(dir) >= n && strncmp(dir, pa, n) == 0) {
		return dir + n;
	} 
	return NULL;
}

/* 向客户端发送文件大小 */
static int send_size(int fd, int size){
	char buf[MAX_LEN + 1] = { 0 };
	strcat(buf, CMD_SIZE_HEADER);
	sprintf(buf + strlen(CMD_SIZE_HEADER), "%d\n", size);
	return write_n(fd, buf, strlen(buf)); 
}

/* 向客户端发送字符串 */
static int send_str(int fd, char* str) {
	char buf[MAX_LEN + 1] = { 0 };
	strcat(buf, CMD_COMMAND_HEADER);
	strcat(buf, str);
	strcat(buf, "\n");
	return write_n(fd, buf, strlen(buf));
} 

/* 向客户端发送结束 */
static int send_fin(int fd) {
	char buf[MAX_LEN + 1] = {0};
	sprintf(buf, "%s\n", CMD_FIN_HEADER);
	return write_n(fd, buf, strlen(buf));
}

int ls(int fd, int argc, char** argv) {
	char ret[MAX_LEN + 1] = { 0 }, buf[MAX_LEN + 1] = { 0 };
	int r = exec(argc, argv, buf);
	
	// 将默认换行符分割改为空格分割以便消息处理
	int len = strlen(buf);
	strcat(ret, ".");
	if (strcmp(PWD, "~") != 0) {
		strcat(ret, " ..");
	}
	strcat(ret, " ");
	for(int i = 0; i < len; i++){
		if(buf[i] == '\n') buf[i] = ' ';
	}
	strcat(ret, buf);
	printf("%s: %s\n", argv[0], ret);
	send_str(fd, ret);
	return 0;
}

int pwd(int fd, int argc, char** argv) {
	printf("%s: %s\n", argv[0], PWD);
	send_str(fd, PWD);
	return 0;
}

int cd(int fd, int argc, char** argv) {
	char buf[MAX_LEN + 1] = { 0 }, ret[MAX_LEN + 1] = { 0 };
	int r = exec(argc, argv, ret);
	printf("%s: %s\n", argv[0], ret);
		// 更改目录
		if(chdir(argv[1]) < 0) {
			sprintf(ret, "can't access '%s': No such file or dictory.", argv[1]);
		}
		// 获取当前新路经
		r = get_pwd(buf);
		// 不在服务器根目录下
		char *new_pwd;
		if ((new_pwd = is_parent(buf, ROOT)) != NULL) {
			// 更新路径
			strcpy(PWD, "~");
			strcat(PWD, new_pwd);
		} else {
			// 更改回原目录
			strcpy(buf, ROOT);
			strcat(buf, PWD + 1);
			r = chdir(buf);
			sprintf(ret, "can't access '%s': No such file or dictory.", argv[1]);
		}
	// 返回消息
	send_str(fd, ret);
	// 向客户端发送新路径	
	send_str(fd, PWD);
	return 0;
}

int Mkdir(int fd, int argc, char** argv) {
	char ret[MAX_LEN + 1] = {0};
	int r = exec(argc, argv, ret);
	printf("%s: %s\n", argv[0], ret);
	r = send_str(fd, ret);
	return 0;
}

int get(int fd, int argc, char** argv) {
	if (argc != 2){
		return  CMD_WRONG_USAGE;
	}
	extern char cmd_msg[];
	extern char cmd_error_msg[];

	char *in_name = argv[1];
	// 打开文件
	int in_fd = open(in_name, O_RDONLY);
	if (in_fd < 0){
		sprintf(cmd_error_msg, "Can't access to '%s': No such file or directory", in_name);
		return CMD_ERROR;
	}

	// 准备缓存区
	buf_io_t buf_io;
	buf_io_init(&buf_io, in_fd);
	char buf[MAX_LEN + 1] = { 0 };
	int n;
	int success_n = 0;

	// 告知发送文件大小
	int in_size = get_file_size(in_name);
	n = send_size(fd, in_size);
	if (n<0){
		sprintf(cmd_error_msg, "Write to client error");
		return CMD_ERROR;
	}

	// 边读文件边写入客户端
	char bar[MAX_LEN + 1] = { 0 };
	while ((n = buf_read_n(&buf_io, buf, MAX_LEN)) != 0){
		if (n < 0){
			sprintf(cmd_error_msg, "Read from file error: %s", in_name);
			return CMD_ERROR;
		}
		// 写入
		n = write_n(fd, buf, n);
		if(n < 0){
			sprintf(cmd_error_msg, "Write to client error");
			return CMD_ERROR;
		}
		success_n += n;
		printf("%s(%d/%d)\n", process_bar((double)success_n / in_size, 40, bar), success_n, in_size);
	}
	// 关闭文件
	close(in_fd);
	// 等待服务端下载结束
	n = wait_header(fd, CMD_FIN_HEADER, NULL, MAX_TIME);
	if (n < 0){
		sprintf(cmd_error_msg, "Client get file failed: time out (max %ds)", MAX_TIME);
		return CMD_ERROR;
	}
	sprintf(cmd_msg, "Successfully put %s!\n", in_name);

	return 0;	
}

int put(int fd, int argc, char **argv) {
	buf_io_t buf_io;
	buf_io_init(&buf_io, fd);
	char buf[MAX_LEN + 1] = {0};
	int n;
	int size;

	extern char cmd_msg[];
	extern char cmd_error_msg[];

	// 阻塞直到收到文件大小
	n = wait_header(fd, CMD_SIZE_HEADER, buf, MAX_TIME);
	if (n < 0)
	{
		sprintf(cmd_error_msg, "Time out (max %d seconds)\n", MAX_TIME);
		return CMD_ERROR;
	}
	size = atoi(buf);
	printf("Ready to get %d bytes\n", size);

	// 写入文件
	char *file_name = argv[1];
	// 去除路径
	get_file_name(&file_name);
	// 打开/创建写入文件
	int out_fd = open(file_name, O_WRONLY | O_CREAT, S_IRWXU);
	// 成功写入的数量
	int success_n = 0;
	// 开始计时
	clock_t start_time = clock();
	while (success_n < size)
	{
		clock_t cur_time = clock();
		if ((double)(cur_time - start_time) / CLOCKS_PER_SEC > MAX_TIME)
		{
			// 超时
			sprintf(cmd_error_msg, "Time out (max %d seconds)\n", MAX_TIME);
			return CMD_ERROR;
		}
		int max_len = MAX_LEN;
		if (max_len > size - success_n)
		{
			max_len = size - success_n;
		}
		n = buf_read_n(&buf_io, buf, max_len);
		if (n < 0)
		{
			sprintf(cmd_error_msg, "Read from client failed\n");
			return CMD_ERROR;
		}
		if (write_n(out_fd, buf, n) != n)
		{
			sprintf(cmd_error_msg, "Write to file failed: %s\n", file_name);
			return CMD_ERROR;
		}
		success_n += n;
		char bar[MAX_LEN + 1];
		// 打印进度条
		printf("%s(%d/%d)\n", process_bar((double)success_n / size, 40, bar), success_n, size);
	}
	// 关闭文件
	close(out_fd);
	// 成功，发送FIN
	printf("Successfully got %d bytes\n", success_n);
	n = send_fin(fd);

	return 0;
}

int Delete(int fd, int argc, char **argv) {
	argv[0] = "rm";
	char ret[MAX_LEN + 1] = { 0 };
	int r = exec(argc, argv, ret);
	send_str(fd, ret);
	return 0;
}

int quit(int fd, int argc, char **argv) {
	return CMD_QUIT;
}
