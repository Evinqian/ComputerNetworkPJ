#include <inc/command.h>
#include <inc/server/connect.h>
#include <inc/io.h>
#include <inc/utils.h>

/* 服务端描述符 */
extern int server_fd;
/* 当前连接文件描述符 */
extern int conn_fd;
char PWD[1024] = ".";
/*向客户端发送文件大小*/
static int send_size(int size){
	char buf[MAX_LEN + 1] = { 0 };
	strcat(buf, CMD_SIZE_HEADER);
	sprintf(buf + strlen(CMD_SIZE_HEADER), "%d\n", size);
	return write_n(conn_fd, buf, strlen(buf)); 
}

/* 向客户端发送结束 */
static int send_fin() {
	char buf[MAX_LEN + 1] = { 0 };
	sprintf(buf, "%s\n", CMD_FIN_HEADER);
	return write_n(conn_fd, buf, strlen(buf));
}

int ls(int argc, char** argv) {
	char ret[1024] = {0};
    FILE* fp;
    fp = popen("cd %s\nls" % PWD,"r");
    fread(ret,1,1024,fp);
	//TODO:向客户端发送ret
	send
	return 0;
}

int pwd(int argc, char** argv) {
	//TODO:向客户端发送PWD
	return 0;
}

int cd(int argc, char** argv) {
	char ret[1024] = {0};
    FILE* fp;
    fp = popen("cd %s\ncd %s" % (PWD, argv[1]),"r");
    fread(ret,1,1024,fp);
	memset(PWD, 0, sizeof(PWD));
	strcpy(PWD, ret);	//更新PWD
	//TODO:向客户端发送ret
	return 0;
}

int Mkdir(int argc, char** argv) {
	char ret[1024] = {0};
    FILE* fp;
    fp = popen("cd %s\nmkdir %s" % (PWD, argv[1]),"r");
    fread(ret,1,1024,fp);
	//TODO:向客户端发送ret
	return 0;
}

int get(int argc, char** argv) {
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
	n = send_size(in_size);
	if (n<0){
		sprintf(cmd_error_msg, "Write to client error");
		return CMD_ERROR;
	}

	// 边读文件边写入客户端
	while ((n = buf_read_n(&buf_io, buf, MAX_LEN)) != 0){
		if (n < 0){
			sprintf(cmd_error_msg, "Read from file error: %s", in_name);
			return CMD_ERROR;
		}
		// 写入
		n = write_n(conn_fd, buf, n);
		if(n < 0){
			sprintf(cmd_error_msg, "Write to client error");
			return CMD_ERROR;
		}
		success_n += n;
	}
	printf("Successfully transmitted %d bytes. Waiting client to get..\n", success_n);
	
	// 等待服务端下载结束
	n = wait_header(conn_fd, CMD_FIN_HEADER, NULL, MAX_TIME);
	if (n < 0){
		sprintf(cmd_error_msg, "Client get file failed: time out (max %ds)", MAX_TIME);
		return CMD_ERROR;
	}
	sprintf(cmd_msg, "Successfully put %s!\n", in_name);

	return 0;	
}

int put(int argc, char** argv) {
	buf_io_t buf_io;
	buf_io_init(&buf_io, conn_fd);
	char buf[MAX_LEN + 1] = { 0 };
	int n;
	int size;

	extern char cmd_msg[];
	extern char cmd_error_msg[];

	// 阻塞直到收到文件大小
	n = wait_header(conn_fd, CMD_SIZE_HEADER, buf, MAX_TIME);
	if (n < 0) {
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
	while (success_n < size) {
		clock_t cur_time = clock();
		if ((double)(cur_time - start_time) / CLOCKS_PER_SEC > MAX_TIME) {
			// 超时
			sprintf(cmd_error_msg, "Time out (max %d seconds)\n", MAX_TIME);
			return CMD_ERROR;
		}
		int max_len = MAX_LEN;
		if (max_len > size - success_n) {
			max_len = size - success_n;
		}
		n = buf_read_n(&buf_io, buf, max_len);
		if (n < 0) {
			sprintf(cmd_error_msg, "Read from client failed\n");
			return CMD_ERROR;
		}
		if (write_n(out_fd, buf, n) != n) {
			sprintf(cmd_error_msg, "Write to file failed: %s\n", file_name);
			return CMD_ERROR;
		}
		success_n += n;
		char bar[MAX_LEN + 1];
		// 打印进度条
		printf("%s(%d/%d)\n", process_bar((double)success_n / size, 40, bar), success_n, size);
	}
	
	// 成功，发送FIN
	printf("Successfully got %d bytes\n", success_n);
	n = send_fin();

	return 0;
}

int Delete(int argc, char** argv) {
	return 0;
}

int quit(int argc, char** argv) {
	return CMD_QUIT;
}

