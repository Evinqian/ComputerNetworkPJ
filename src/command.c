#include <inc/command.h>
#include <inc/client/connect.h>
#include <inc/io.h>
#include <string.h>
#include <sys/types.h>    
#include <sys/stat.h>    
#include <fcntl.h>

// 错误信息
char cmd_msg[MAX_LEN];
char cmd_error_msg[MAX_LEN];

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
	char buf_cpy[MAX_LINE] = { 0 };
	memcpy(buf_cpy, buf, strlen(buf));
	if (parse(buf_cpy, argc, argv) == CMD_TOO_MANT_ARGS) {
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