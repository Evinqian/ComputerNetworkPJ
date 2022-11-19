#include <inc/command.h>
#include <inc/io.h> 
#include <inc/utils.h> 

// 错误信息
char cmd_msg[MAX_LEN];
char cmd_error_msg[MAX_LEN];

struct Command {
	const char *name;
	const char *desc;
	int (*func)(int fd, int argc, char** argv);
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

char *process_bar(double process, int length, char *buf) {
	memset(buf, 0, sizeof(buf));
	if (process < 0) {
		process = 0;
	} else if (process > 1) {
		process = 1;
	}
	int i;
	strcat(buf, "[");
	for (i = 0; i < length * process - 1; i++) {
		strcat(buf, "=");
	}
	strcat(buf, ">");
	for (i++; i < length; i++) {
		strcat(buf, " ");
	}
	strcat(buf, "]");
	return buf;
}

/* 解析命令 */
static int parse_command(char *buf, int *argc, char **argv) {
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

/* 寻找匹配的命令 */
static struct Command *match(char *name) {
	int i;
	for (i = 0; i < sizeof(commands) / sizeof(struct Command); i++) {
		if (strcmp(name, commands[i].name) == 0) {
			return &commands[i];
		}
	}
	return NULL;
}

int exec(int argc, char **argv, char *ret) {
	char buf[MAX_LEN + 1] = { 0 };
	sprintf(buf, "%s", argv[0]);
	for (int i = 1; i < argc; i++) {
		strcat(buf, " ");
		strcat(buf, argv[i]);
	}

	FILE* fp = popen(buf, "r");
	int n = 0;
	if (ret != NULL) { 
		n = fread(ret, 1, MAX_LEN, fp);
	}
	if (n < 0) {
		return -1;
	}
	return 0;
}

char *check_header(char *buf, const char *header) {
	char tmp[MAX_LEN + 1] = { 0 };
	int n = strlen(header);
	if (strlen(buf) < n) {
		return NULL;
	}
	strncpy(tmp, buf, n);
	if (strcmp(tmp, header) == 0) {
		return buf + n;
	}
	return NULL;
}

int wait_header(int fd, const char *header, char *data, int seconds){
	int n;
	char buf[MAX_LEN + 1] = { 0 };
	int left = 0, right = 0;
	char *tmp = NULL;
	clock_t start_time = clock();
	while (1) {
		clock_t cur_time = clock();
		if (seconds >= 0 && (double)(cur_time - start_time) / CLOCKS_PER_SEC > seconds) {
			return -1;
		}
        n = recv(fd, buf + right, 1, MSG_DONTWAIT);
		if (n > 0) {
			if (buf[right] == '\n') {
				if ((tmp = check_header(buf + left, header)) != NULL) {
					*strchr(tmp, '\n') = 0;
					if (data) {
						strcpy(data, tmp);
					}
					return 0;
				} else {
					// 丢弃上一段
					left = (right + n) % MAX_LEN;
				}
			}
			right = (right + n) % MAX_LEN;
		} else if (n == 0) {
			return -1;
		}
    }
}

void add_header(char* buf, const char *header) {
	char tmp[MAX_LEN + 1] = { 0 };
	strcat(tmp, header);
	strcat(tmp, buf);
	strcpy(buf, tmp);
}

int print_command_usage(char *name) {
	struct Command *command = match(name);
	if (command == NULL) {
		return CMD_UNKNOWN;
	}
	printf("%s", command->desc);
	return 0;
}

int run_command(int fd, char *buf, int* argc, char** argv) {
	char buf_cpy[MAX_LINE] = { 0 };
	memcpy(buf_cpy, buf, strlen(buf));
	if (parse_command(buf_cpy, argc, argv) == CMD_TOO_MANT_ARGS) {
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
	return command->func(fd, *argc, argv);
}