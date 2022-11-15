#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#define BUF_SIZE 1024

typedef struct {
    int fd;
    int n;              /* 缓存区中未读的字节数 */
    char *cur;      /* 缓存区当前将要读的位置 */
    char buf[BUF_SIZE]; /* 缓存区 */
} buf_io_t;

int read_n(int fd, void *buf, size_t n);
int write_n(int fd, void *buf, size_t n);
void buf_io_init(buf_io_t *buf_io, int fd);
int buf_read_n(buf_io_t *rp, char *buf, size_t n);
int buf_read_line(buf_io_t *buf_io, char *buf, size_t maxlen);
