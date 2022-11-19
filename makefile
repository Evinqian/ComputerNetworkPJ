CC	:= $(GCCPREFIX)gcc -pipe
PY := python
CFLAGS := $(CFLAGS) -O2 -fno-builtin -I . -MD
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -std=gnu99
CFLAGS += -Wall -Wno-format -Wno-unused -Werror -gstabs -m32

OBJ_DIR := obj
SRC_DIR := src
INC_DIR := inc
TEST_DIR := test
TEST := $(TEST_DIR)/test.py
SERVER_DIR := server
CLIENT_DIR := client

FTP := ftp
FTP_SERVER := $(SERVER_DIR)/ftp-server

# TODO
# 完成新的.c文件后需要在这里补充文件名 
SERVER_SRC_FILES :=  src/utils.c src/io.c src/command.c src/server/main.c src/server/command.c
CLIENT_SRC_FILES :=  src/utils.c src/io.c src/command.c src/client/main.c src/client/command.c

SERVER_OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SERVER_SRC_FILES))
CLIENT_OBJ_FILES := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(CLIENT_SRC_FILES))
all: $(FTP) $(FTP_SERVER)

$(FTP): $(CLIENT_OBJ_FILES)
	@echo + cc[CLIENT] $^
	$(CC) $(CFLAGS) -o $@ $^

$(FTP_SERVER): $(SERVER_OBJ_FILES)
	@echo + cc[SERVER] $^
	$(CC) $(CFLAGS) -o $@ $^ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo + cc[SERVER] $^
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $^

test: all
	@echo $(MAKE) clean
	@$(MAKE) clean || echo "Nothing to clean"
	$(PY) ./$(TEST)

clean:
	rm -r $(OBJ_DIR)/* $(FTP) $(FTP_SERVER)