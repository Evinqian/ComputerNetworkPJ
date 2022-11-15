#include <inc/client/command.h>
#include <inc/client/connect.h>
#include <inc/io.h>
#include <string.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>

struct Command {
	const char *name;
	const char *desc;
	int (*func)(int argc, char** argv);
};

static struct Command commands[] = {
	{ "ls", "ls", ls },
	{ "pwd", "pwd", pwd },
	{ "cd", "cd [remote_direcotry_name]", cd },
	{ "mkdir", "mkdir [remote_direcotry_name]", Mkdir },
	{ "get", "get [remote_filename]", get },
	{ "put", "put [local_filename]", put },
	{ "delete", "delete [remote_filename]", Delete },
	{ "quit", "quit", quit }
};

#define WHITE_SPACE "\t\r\n "

static int parse(char *buf, int *argc, char **argv) {
	while (1) {
		while (*buf && strchr(WHITE_SPACE, *buf)) {
			*buf++ = 0;
		}
		if (*buf == 0) {
			break;
		}
		if (*argc == MAX_ARGC - 1) {		// 参数太多
			return CMD_TOO_MANT_ARGS;;
		}
		argv[(*argc)++] = buf;
		while (*buf && !strchr(WHITE_SPACE, *buf)) {
			buf++;
		}
	}
	argv[*argc] = 0;
	return 0;
}

static struct Command *match(char *name) {
	int i;
	for (i = 0; i < sizeof(commands) / sizeof(struct Command); i++) {
		if (strcmp(name, commands[i].name) == 0) {
			return &commands[i];
		}
	}
	return NULL;
}

int print_usage(char *name) {
	struct Command *command = match(name);
	if (command == NULL) {
		return CMD_UNKNOWN;
	}
	printf("%s", command->desc);
	return 0;
}

int run(char *buf, int* argc, char** argv) {
	if (parse(buf, argc, argv) == CMD_TOO_MANT_ARGS) {
		return CMD_TOO_MANT_ARGS;
	}
	// 清空信息
	strcpy(cmd_error_msg, "");
	strcpy(cmd_msg, "");
	int i;
	// 遍历寻找匹配的函数
	struct Command *command = match(argv[0]);
	if (command == NULL) {
		return CMD_UNKNOWN;
	}
	return command->func(*argc, argv);
}

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
	extern int client_fd;
	if (argc != 2) {
		return CMD_WRONG_USAGE;
	}
	// 打开文件
	char *in_name = argv[1];
	int in_fd = open(in_name, O_RDONLY);
	if (in_fd < 0) {
		sprintf(cmd_error_msg, "can't access to '%s': No such file or directory", in_name);
		return CMD_ERROR;
	}
	// 去除路径
	while (1) {
		char *t = strchr(in_name, '/');
		if (t == NULL) {
			break;
		}
		in_name = ++t;
	}

	// TODO
	// 名字过长有隐患
	char out_name[MAX_LEN] = "out_";
	strcat(out_name, in_name);
	int out_fd = open(out_name, O_WRONLY | O_CREAT, S_IRWXU);

	buf_io_t buf_io;
	buf_io_init(&buf_io, in_fd);
	char buf[MAX_LEN];
	int n;
	int tot = 0;
	while ((n = buf_read_n(&buf_io, buf, MAX_LEN)) != 0) {
		if (n < 0) {
			sprintf(cmd_error_msg, "Read error");
			return CMD_ERROR;
		}
		n = write_n(out_fd, buf, n);
		tot += n;
	}
	sprintf(cmd_msg, "Successfully tranmitted %d bytes.", tot);
	return 0;
}

int Delete(int argc, char** argv) {
	return 0;
}

int quit(int argc, char** argv) {
	return CMD_QUIT;
}

