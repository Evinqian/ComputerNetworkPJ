#include <inc/command.h>
#include <inc/client/connect.h>
#include <inc/io.h>
#include <inc/utils.h>

/* 客户端文件描述符 */
extern int client_fd;

/* 向服务端发送命令 */
static int send_command(int argc, char** argv) {
	char cmd[MAX_LEN + 1] = { 0 };
	int i;
	strcat(cmd, CMD_COMMAND_HEADER);
	strcat(cmd, argv[0]);
	for (i = 1; i < argc; i++) {
		strcat(cmd, " ");
		strcat(cmd, argv[i]);
	}
	strcat(cmd, "\n");
	return write_n(client_fd, cmd, strlen(cmd));
}

/* 向服务端发送文件大小 */
static int send_size(int size) {
	char buf[MAX_LEN + 1] = { 0 };
	strcat(buf, CMD_SIZE_HEADER);
	sprintf(buf + strlen(CMD_SIZE_HEADER), "%d\n", size);
	return write_n(client_fd, buf, strlen(buf));
}

int ls(int argc, char** argv) {
	int r = send_command(argc, argv);
	return 0;
}

int pwd(int argc, char** argv) {
	int r = send_command(argc, argv);
	return 0;
}

int cd(int argc, char** argv) {
	int r = send_command(argc, argv);
	return 0;
}

int Mkdir(int argc, char** argv) {
	int r = send_command(argc, argv);
	return 0;
}

int get(int argc, char** argv) {
	int r = send_command(argc, argv);
	return 0;
}

int put(int argc, char** argv) {
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
	n = send_command(argc, argv);
	if (n < 0) {
		sprintf(cmd_error_msg, "Write to server error");
		return CMD_ERROR;
	}
	
	// 告知发送文件大小
	int in_size = get_file_size(in_name);
	n = send_size(in_size);
	if (n < 0) {
		sprintf(cmd_error_msg, "Write to server error");
		return CMD_ERROR;
	}

	// 边读文件边写入服务端
	while ((n = buf_read_n(&buf_io, buf, MAX_LEN)) != 0) {
		if (n < 0) {
			sprintf(cmd_error_msg, "Read from file error: %s", in_name);
			return CMD_ERROR;
		}
		// 写入
		n = write_n(client_fd, buf, n);
		if (n < 0) {
			sprintf(cmd_error_msg, "Write to server error");
			return CMD_ERROR;
		}
		success_n += n;
	}
	sprintf(cmd_msg, "Successfully tranmitted %d bytes.", success_n);
	return 0;
}

int Delete(int argc, char** argv) {
	int r = send_command(argc, argv);
	return 0;
}

int quit(int argc, char** argv) {
	return CMD_QUIT;
}

