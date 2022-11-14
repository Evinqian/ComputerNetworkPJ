CC	:= $(GCCPREFIX)gcc -pipe
PY := python
CFLAGS := $(CFLAGS) -O2 -fno-builtin -I . -MD
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -std=gnu99
CFLAGS += -static
CFLAGS += -Wall -Wno-format -Wno-unused -Werror -gstabs -m32

OBJ_DIR := obj
SRC_DIR := src
INC_DIR := inc
TEST_DIR := test
TEST := $(TESTDIR)/test.py
SERVER_DIR := server
CLIENT_DIR := client

# TODO
# 完成新的.c文件后需要在这里补充文件名
SERVER_SRC_FILES :=  src/server/main.c
CLIENT_SRC_FILES :=  src/client/main.c

SERVER_OBJ_FILES := $(patsubst $(SRC_DIR)/$(SERVER_DIR)/%.c, $(OBJ_DIR)/$(SERVER_DIR)/%.o, $(SERVER_SRC_FILES))
CLIENT_OBJ_FILES := $(patsubst $(SRC_DIR)/$(CLIENT_DIR)/%.c, $(OBJ_DIR)/$(CLIENT_DIR)/%.o, $(CLIENT_SRC_FILES))

all: ftp ftp-server

ftp: $(CLIENT_OBJ_FILES)
	@echo + cc[CLIENT] $<
	$(CC) $(CFLAGS) -o $@ $< 

ftp-server: $(SERVER_OBJ_FILES)
	@echo + cc[SERVER] $<
	$(CC) $(CFLAGS) -o $@ $< 

$(OBJ_DIR)/$(CLIENT_DIR)/%.o: $(SRC_DIR)/$(CLIENT_DIR)/%.c
	@echo + cc[CLIENT] $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJ_DIR)/$(SERVER_DIR)/%.o: $(SRC_DIR)/$(SERVER_DIR)/%.c
	@echo + cc[SERVER] $<
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

test: all
	@echo $(MAKE) clean
	@$(MAKE) clean || echo "Nothing to clean"
	$(PY) ./$(TEST)

clean:
	rm -r $(OB_JDIR)/*.o ftp*