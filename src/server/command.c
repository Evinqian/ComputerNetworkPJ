#include <inc/command.h>
#include <inc/server/connect.h>
#include <inc/io.h>
#include <inc/utils.h>

/* 服务端描述符 */
extern int server_fd;
/* 当前连接文件描述符 */
extern int conn_fd;
char PWD[1024] = ".";
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

