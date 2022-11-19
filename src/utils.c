#include <inc/utils.h>

int get_file_size(char *file_name) {
    struct stat st;
	int r = stat(file_name, &st);
	if (r < 0) {
		return -1;
	}
	return st.st_size;
}

void get_file_name(char **file_path) {
	while (1) {
		char *t = strchr(*file_path, '/');
		if (t == NULL) {
			break;
		}
		*file_path = ++t;
	}
}