CC = gcc
CFLAGS = -std=c99 -I $(LIB_DIR)
SRC_DIR = src
GEN_DIR = generate
BIN_DIR = bin
OBJ_DIR = obj
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
INPUT = testcase/1.1.c

$(TARGET): $(OBJS) $(LFO) $(YFO)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) $(YFO) $(CFLAGS) -ll -ly -o $(TARGET)

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@

-include $(DFILES)

$(LFO): $(LFC)
	$(CC) -c $(LFC) -o $(LFO) $(CFLAGS)

$(YFO): $(YFC)
	$(CC) -c $(YFC) -o $(YFO) $(CFLAGS)

$(LFC): $(LFILE)
	@mkdir -p $(GEN_DIR)
	flex -o $(LFC) $(LFILE)

$(YFC): $(YFILE) $(LFC)
	@mkdir -p $(GEN_DIR)
	bison -o $(YFC) -d -v $(YFILE)

.PHONY: run clean

run: $(TARGET)
	$(TARGET) $(INPUT)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(GEN_DIR)


