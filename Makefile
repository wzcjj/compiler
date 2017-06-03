#Copyright © 2017 wzcjj, Nanjing university
CC = gcc
CFLAGS = -Wall -Werror -I $(LIB_DIR)
SRC_DIR = src
GEN_DIR = generate
BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj
LIB_DIR = include
CFILES = $(shell find $(SRC_DIR) -name "*.c")
DFILES = $(shell find . -name "*.d")
OBJS = $(CFILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LFILE = $(SRC_DIR)/lexical.l
YFILE = $(SRC_DIR)/syntax.y
LFC = $(GEN_DIR)/lex.yy.c
YFC = $(GEN_DIR)/syntax.tab.c
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)
TARGET = $(BIN_DIR)/parser
TESTFILE = 3.4
INPUT = testcase/$(TESTFILE).c
OUTPUT = output.txt
#OUTPUT = testout/$(TESTFILE).txt

# git config
GITFLAGS = -q --author='wzcjj <438459483@qq.com>' --no-verify --allow-empty

# prototype: git_commit(msg)
define git_commit
 	-@git add . -A --ignore-errors
	-@while (test -e .git/index.lock); do sleep 0.1; done
	-@(echo "> $(1)" && echo 141250011 && id -un && uname -a && uptime && (head -c 20 /dev/urandom | hexdump -v -e '"%02x"') && echo) | git commit -F - $(GITFLAGS)
endef

$(TARGET): $(OBJS) $(LFO) $(YFO)
	@mkdir -p $(@D)
	$(CC) $(OBJS) $(YFO) $(CFLAGS) -ll -ly -o $(TARGET)

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS)

$(LFO): $(LFC)
	$(CC) -c $(LFC) -o $(LFO) -I $(LIB_DIR)

$(YFO): $(YFC)
	$(CC) -c $(YFC) -o $(YFO) -I $(LIB_DIR)

$(LFC): $(LFILE) $(YFC)
	@mkdir -p $(@D)
	flex -o $(LFC) $(LFILE)

$(YFC): $(YFILE)
	@mkdir -p $(@D)
	bison -o $(YFC) -d -v $(YFILE)

-include $(DFILES)

.PHONY: run clean submit push commit test

run: $(TARGET)
	$(TARGET) $(INPUT) $(OUTPUT)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(GEN_DIR) output.txt testout

submit: clean
	cd .. && tar cvj $(shell pwd | grep -o '[^/]*$$') > 141250011_lab2.tar.gz

push:
	$(call git_commit, "push")
	@git push

commit:
	@git add .
	@git commit --allow-empty

test: $(TARGET)
	@mkdir -p testout
	./test.sh
