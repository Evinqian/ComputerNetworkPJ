#include <inc/client/command.h>
#include <string.h>

struct Command {
	const char *name;
	const char *desc;
	int (*func)(int argc, char** argv);
};

static struct Command commands[] = {
	{ "ls", "", ls },
	{ "pwd", "", pwd },
	{ "cd", "", cd },
	{ "mkdir", "", mkdir },
	{ "get", "", get },
	{ "put", "", put },
	{ "delete", "", del },
	{ "quit", "", quit }
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

int run(char *buf, int* argc, char** argv) {
	if (parse(buf, argc, argv) == CMD_TOO_MANT_ARGS) {
		return CMD_TOO_MANT_ARGS;
	}
	int i;
	// 遍历寻找匹配的函数
	for (i = 0; i < sizeof(commands) / sizeof(struct Command); i++) {
		if (strcmp(argv[0], commands[i].name) == 0) {
			return commands[i].func(*argc, argv);
		}
	}
	return CMD_UNKNOWN;
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

int mkdir(int argc, char** argv) {
	return 0;
}

int get(int argc, char** argv) {
	return 0;
}

int put(int argc, char** argv) {
	return 0;
}

int del(int argc, char** argv) {
	return 0;
}

int quit(int argc, char** argv) {
	return CMD_QUIT;
}

