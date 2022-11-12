CC	:= $(GCCPREFIX)gcc -pipe
CFLAGS := $(CFLAGS) -O2 -fno-builtin -I$(TOP) -MD
CFLAGS += -fno-omit-frame-pointer
CFLAGS += -std=gnu99
CFLAGS += -static
CFLAGS += -Wall -Wno-format -Wno-unused -Werror -gstabs -m32

OBJDIR := obj
SRCDIR := src
TESTDIR := test
TEST := $(TESTDIR)/test.py
PY := python

# TODO
# 完成新的.c文件后需要在这里补充文件名
SRCFILES := src/main.c 

OBJFILES := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCFILES))

all: ftp

ftp: $(OBJFILES)
	@echo + cc[USER] $<
	$(CC) $(CFLAGS) -o $@ $< 

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo + cc[USER] $<
	$(CC) $(CFLAGS) -o $@ -c $< 

test: all
	@echo $(MAKE) clean
	@$(MAKE) clean || echo "Nothing to clean"
	$(PY) ./$(TEST)

clean:
	rm -r $(OBJDIR)/*.o ftp