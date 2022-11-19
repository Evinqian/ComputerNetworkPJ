#include <inc/command.h>
#include <inc/client/connect.h>
#include <inc/io.h>
#include <inc/utils.h>

/* 向服务端发送命令 */
static int send_command(int fd, int argc, char** argv) {
	char cmd[MAX_LEN + 1] = { 0 };
	int i;
	strcat(cmd, CMD_COMMAND_HEADER);
	strcat(cmd, argv[0]);
	for (i = 1; i < argc; i++) {
		strcat(cmd, " ");
		strcat(cmd, argv[i]);
	}
	strcat(cmd, "\n");
	return write_n(fd, cmd, strlen(cmd));
}

/* 向服务端发送文件大小 */
static int send_size(int fd, int size) {
	char buf[MAX_LEN + 1] = { 0 };
	strcat(buf, CMD_SIZE_HEADER);
	sprintf(buf + strlen(CMD_SIZE_HEADER), "%d\n", size);
	return write_n(fd, buf, strlen(buf));
}

/* 向客户端发送结束 */
static int send_fin(int fd) {
	char buf[MAX_LEN + 1] = { 0 };
	sprintf(buf, "%s\n", CMD_FIN_HEADER);
	return write_n(fd, buf, strlen(buf));
}

int ls(int fd, int argc, char** argv) {
	if (argc != 1) {
		return CMD_WRONG_USAGE;
	}
	char buf[MAX_LEN + 1] = { 0 };
	extern char cmd_msg[];
	extern char cmd_error_msg[];

	if(send_command(fd, argc, argv) < 0 
	|| wait_header(fd, CMD_COMMAND_HEADER, buf, MAX_TIME) < 0){
		sprintf(cmd_error_msg, "ls failed");
		return CMD_ERROR;
	}

	if (strlen(buf) != 0) {
		printf("%s\n", buf);
	}
	return 0;
}

int pwd(int fd, int argc, char** argv) {
	if (argc != 1) {
		return CMD_WRONG_USAGE;
	}
	
	char buf[MAX_LEN + 1] = { 0 };
	extern char cmd_msg[];
	extern char cmd_error_msg[];

	if(send_command(fd, argc, argv) < 0
	|| wait_header(fd, CMD_COMMAND_HEADER, buf, MAX_TIME)){
		sprintf(cmd_error_msg, "pwd failed");
		return CMD_ERROR;
	}
	if (strlen(buf) != 0) {
		printf("%s\n", buf);
	}
	return 0;
}

int cd(int fd, int argc, char** argv) {
	if (argc != 2) {
		return CMD_WRONG_USAGE;
	}

	char buf[MAX_LEN + 1] = { 0 };
	extern char cmd_msg[];
	extern char cmd_error_msg[];
	extern char PWD[];

	if(send_command(fd, argc, argv) < 0
	|| wait_header(fd, CMD_COMMAND_HEADER, buf, MAX_TIME) < 0) {
		sprintf(cmd_error_msg, "cd failed");
		return CMD_ERROR;
	}
	// 路径不可访问
	int error = 0;
	if (strlen(buf) != 0) {
		sprintf(cmd_error_msg, "%s", buf);
		error = 1;
	}
	// 更新当前路径
	int n =  wait_header(fd, CMD_COMMAND_HEADER, buf, MAX_TIME);
	strcpy(PWD, buf);

	return error ? CMD_ERROR : 0;
}

int Mkdir(int fd, int argc, char** argv) {
	if (argc != 2) {
		return CMD_WRONG_USAGE;
	}

	char buf[MAX_LEN + 1] = { 0 };
	extern char cmd_msg[];
	extern char cmd_error_msg[];

	if(send_command(fd, argc, argv)  < 0
	|| wait_header(fd, CMD_COMMAND_HEADER, buf, MAX_TIME) < 0){
		sprintf(cmd_error_msg, "mkdir failed");
		return CMD_ERROR;
	}
	// 新建文件夹失败(权限)
	if (strlen(buf) != 0) {
		printf("%s\n", buf);
	}
	return 0;
}

int get(int fd, int argc, char** argv) {
	if (argc != 2) {
		return CMD_WRONG_USAGE;
	}
	buf_io_t buf_io;
	buf_io_init(&buf_io, fd);
	char buf[MAX_LEN + 1] = { 0 };
	int n;
	int size;

	extern char cmd_msg[];
	extern char cmd_error_msg[];

	if (argc != 2) {
		return CMD_WRONG_USAGE;
	}

	int r = send_command(fd, argc, argv);
	  
	// 阻塞直到找到文件
	n = wait_header(fd, CMD_SIZE_HEADER, buf, MAX_TIME);
	if (n < 0){
		sprintf(cmd_error_msg, "File not found");
		return CMD_ERROR;
	}
	size = atoi(buf);
	printf("Ready to get %d bytes\n",size);

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
	while (success_n < size){
		clock_t cur_time = clock();
		if ((double)(cur_time - start_time) / CLOCKS_PER_SEC > MAX_TIME){
			//超时
			sprintf(cmd_error_msg, "Time out (max %d second)\n", MAX_TIME);
			return CMD_ERROR;
		}
		int max_len = MAX_LEN;
		if (max_len > size - success_n){
			max_len = size - success_n;
		}
		n = buf_read_n(&buf_io, buf, max_len);
		if (n < 0){
			sprintf(cmd_error_msg, "Read from server failed\n");
			return CMD_ERROR;
		}
		if (write_n(out_fd, buf, n) != n){
			sprintf(cmd_error_msg, "Write to file failed: %s\n", file_name);
			return CMD_ERROR;
		}
		success_n += n;
		char bar[MAX_LEN + 1];
		//打印进度条
		printf("%s(%d/%d)\n", process_bar((double)success_n / size,40, bar), success_n, size);
	}
	// 关闭文件
	n = close(out_fd);
	// 成功，发送FIN
	printf("Successfully got %d bytes\n", success_n);
	n = send_fin(fd);

	return 0;
}

int put(int fd, int argc, char** argv) {
	if (argc != 2) {
		return CMD_WRONG_USAGE;
	}
	extern char cmd_msg[];
	extern char cmd_error_msg[];

	char *in_name = argv[1];
	// 打开文件
	int in_fd = open(in_name, O_RDONLY);
	if (in_fd < 0) {
		sprintf(cmd_error_msg, "Can't access to '%s': No such file or directory", in_name);
		return CMD_ERROR;
	}

	// 准备缓存区
	buf_io_t buf_io;
	buf_io_init(&buf_io, in_fd);
	char buf[MAX_LEN + 1] = { 0 };
	int n;
	int success_n = 0;

	// 将命令发送给服务端
	n = send_command(fd, argc, argv);
	if (n < 0) {
		sprintf(cmd_error_msg, "Write to server error");
		return CMD_ERROR;
	}
	
	// 告知发送文件大小
	int in_size = get_file_size(in_name);
	n = send_size(fd, in_size);
	if (n < 0) {
		sprintf(cmd_error_msg, "Write to server error");
		return CMD_ERROR;
	}

	// 边读文件边写入服务端
	char bar[MAX_LEN + 1] = { 0 };
	while ((n = buf_read_n(&buf_io, buf, MAX_LEN)) != 0) {
		if (n < 0) {
			sprintf(cmd_error_msg, "Read from file error: %s", in_name);
			return CMD_ERROR;
		}
		// 写入
		n = write_n(fd, buf, n);
		if (n < 0) {
			sprintf(cmd_error_msg, "Write to server error");
			return CMD_ERROR;
		}
		success_n += n;
		printf("%s(%d/%d)\n", process_bar((double)success_n / in_size, 40, bar), success_n, in_size);
	}
	// 关闭文件
	n = close(in_fd);
	// 等待服务端下载结束
	n = wait_header(fd, CMD_FIN_HEADER, NULL, MAX_TIME);
	if (n < 0) {
		sprintf(cmd_error_msg, "Server get file failed: time out (max %ds)", MAX_TIME);
		return CMD_ERROR;
	}
	sprintf(cmd_msg, "Successfully put %s!\n", in_name);

	return 0;
}

int Delete(int fd, int argc, char** argv) {
	if (argc != 2) {
		return CMD_WRONG_USAGE;
	}
	
	char buf[MAX_LEN + 1] = { 0 };
	extern char cmd_msg[];
	extern char cmd_error_msg[];

	if (send_command(fd, argc, argv) < 0
	|| wait_header(fd, CMD_COMMAND_HEADER, buf, MAX_TIME) < 0) {
		sprintf(cmd_error_msg, "delete failed");
		return CMD_ERROR;
	}
	if (strlen(buf) != 0) {
		printf("%s\n", buf);
	}
	return 0;
}

int quit(int fd, int argc, char** argv) {
	if (argc != 1) {
		return CMD_WRONG_USAGE;
	}
	return CMD_QUIT;
}

