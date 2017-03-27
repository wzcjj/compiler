CC = gcc
CFLAGS = -std=c99
SRC_DIR = src
GEN_DIR = generate
BIN_DIR = bin
OBJ_DIR = obj
CFILES = $(shell find $(SRC_DIR) -name "*.c")
DFILES = $(shell find . -name "*.d")
OBJS = $(CFILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LFILE = $(SRC_DIR)/lexical.l
YFILE = $(SRC_DIR)/syntax.y
LFC = $(GEN_DIR)/lex.yy.c
YFC = $(GEN_DIR)/syntax.tab.c
LFO = $(LFC:.c=,o)
YFO = $(YFC:.c=.o)
TARGET = $(BIN_DIR)/parser

$(TARGET): $(OBJS) $(LFO) $(YFO)
	$(CC) $(OBJS) $(LFO) $(YFO) $(CFLAGS) -ll -o $(TARGET)

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir $(OBJ_DIR)
	$(CC) -c $(CFILES) -o $(OBJ_DIR)

-include $(DFILES)

$(LFO): $(LFC)
	$(CC) -c $(LFC) -o $(LFO)

$(YFO): $(YFC)
	$(CC) -c $(YFC) -o $(YFO)

$(LFC): $(LFILE)
	@mkdir $(GEN_DIR)
	flex $(LFILE) -o $(LFC)

$(YFC): $(YFILE)
	@mkdir $(GEN_DIR)
	bison -d -v $(YFILE) -o $(YFC)

.PHONY: run clean

run: $(TARGET)
	$(TARGET) $(INPUT)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(GEN_DIR)


