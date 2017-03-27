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
LFO = $(LFC:.c=.o)
YFO = $(YFC:.c=.o)
TARGET = $(BIN_DIR)/parser

$(TARGET): $(OBJS) $(LFO) $(YFO)
	$(CC) $(OBJS) $(LFO) $(YFO) $(CFLAGS) -ll -o $(TARGET)

$(OBJS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) -c $< -o $@

-include $(DFILES)

$(LFO): $(LFC)
	$(CC) -c $(LFC) -o $(LFO)

$(YFO): $(YFC)
	$(CC) -c $(YFC) -o $(YFO)

$(LFC): $(LFILE)
	@mkdir -p $(GEN_DIR)
	flex -o $(LFC) $(LFILE)

$(YFC): $(YFILE)
	@mkdir -p $(GEN_DIR)
	bison -o $(YFC) -d -v $(YFILE)

.PHONY: run clean

run: $(TARGET)
	$(TARGET) $(INPUT)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) $(GEN_DIR)


