#include <inc/io.h>

int read_n(int fd, void *buf, size_t n) {
    int n_left = n;
    int n_read;
    char *buf_tmp = buf;

    while (n_left > 0) {
        if ((n_read = read(fd, buf, n_left)) < 0) {
            if (errno == EINTR) {   // 系统调用失败 
                n_read = 0;         // 无事发生，再次调用
            } else {
                return -1;           
            }
        } else if (n_read == 0) {
            break;                  // EOF
        }
        n_left -= n_read;
        buf_tmp += n_read;
    }
    return (n - n_left);
}

int write_n(int fd, void *buf, size_t n) {
    int n_left = n;
    int n_written;
    char *buf_tmp = buf;

    while (n_left > 0) {
        if ((n_written = write(fd, buf_tmp, n_left)) <= 0) {
            if (errno == EINTR) {   // 系统调用失败 
                n_written = 0;      // 无事发生，再次调用
            } else {
                return -1;
            }
        }
        n_left -= n_written;
        buf_tmp += n_written;
    }
    return n;
}

void buf_io_init(buf_io_t *buf_io, int fd) {
    buf_io->fd = fd;
    buf_io->n = 0;
    buf_io->cur = buf_io->buf;
}

static int buf_read(buf_io_t *buf_io, char* buf, size_t n) {
    while (buf_io->n <= 0) {
        buf_io->n = read(buf_io->fd, buf_io->buf, sizeof(buf_io->buf));
        if (buf_io->n < 0) {
            if (errno != EINTR) {
                return -1;
            }
        } else if (buf_io->n == 0) {    /* EOF */
            return 0;
        } else {
            buf_io->cur = buf_io->buf;
        }
    }

    /* 从缓存区复制min(buf_io->n, n)个字节 */
    int cnt = n;
    if (buf_io->n < n) {
        cnt = buf_io->n;
    }
    memcpy(buf, buf_io->cur, cnt);
    buf_io->cur += cnt;
    buf_io->n -= cnt;
    return cnt;
}

int buf_read_n(buf_io_t *rp, char *buf, size_t n) {
    int n_left = n;
    int n_read;
    char *buf_tmp = buf;
    
    while (n_left > 0) {
	    if ((n_read = buf_read(rp, buf_tmp, n_left)) < 0) {
            return -1; 
        } else if (n_read == 0) {
	        break;              /* EOF */
        }
        n_left -= n_read;
	    buf_tmp += n_read;
    }
    return (n - n_left);
}

int buf_read_line(buf_io_t *buf_io, char *buf, size_t maxlen) {
    int i, n;
    char c, *buf_tmp = buf;

    for (i = 0; i < maxlen - 1; i++) {
        /* 逐字符读入 */ 
        if ((n = buf_read(buf_io, &c, 1)) == 1) {
            *buf_tmp++ = c;
            if (c == '\n') {
                i++;
                break;
            }
        } else if (n == 0) {    
            if (i == 0) {   /* 没有可读数据 */
                return 0;
            } else {
                break;
            }
        } else {
            return -1;
        }
    }
    /* 末尾填充空字符'\0' */
    *buf_tmp = 0;
    return i;
}