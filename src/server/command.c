#include <inc/command.h>
#include <inc/server/connect.h>
#include <inc/io.h>
#include <string.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <sys/socket.h>    
#include <fcntl.h>
#include <inc/utils.h>

/* 服务端描述符 */
extern int server_fd;
/* 当前连接文件描述符 */
extern int conn_fd;

int ls(int argc, char** argv) {
	return 0;
}

int pwd(int argc, char** argv) {
	return 0;
}

int cd(int argc, char** argv) {
	return 0;
}

int Mkdir(int argc, char** argv) {
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


	n = buf_read_line(&buf_io, buf, MAX_LINE);
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
	while (success_n < size) {
		int max_len = MAX_LEN;
		if (max_len > size - success_n) {
			max_len = size - success_n;
		}
		n = buf_read_n(&buf_io, buf, max_len);
		if (n < 0) {
			sprintf(cmd_error_msg, "Read from client failed\n");
			return -1;
		}
		if (write_n(out_fd, buf, n) != n) {
			sprintf(cmd_error_msg, "Write to file failed: %s\n", file_name);
			return -1;
		}
		success_n += n;
		printf("Got %d bytes (%d/%d)\n", n, success_n, size);
	}
	
	// 成功
	printf("Successfully get %d bytes\n", success_n);
	return 0;
}

int Delete(int argc, char** argv) {
	return 0;
}

int quit(int argc, char** argv) {
	return CMD_QUIT;
}

