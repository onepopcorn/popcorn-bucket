SRC_DIR := ./
BIN := ../../out/$(FILENAME)
OBJ_DIR := ./obj

SRCS	:= $(shell find $(SRC_DIR) -name '*.c')
OBJS	:= $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRCS))

CFLAGS	:= -I. -c -std=gnu99 -Wall -Werror -pedantic -O3 -fomit-frame-pointer -ffast-math -march=i386

all: $(BIN)
	@echo "CREATED out/$(FILENAME)"

clean:
	@rm -rf $(BIN) $(OBJ_DIR)/*

$(BIN): $(OBJS)
	@echo "BUILDING out/$(FILENAME)"
	@mkdir -p $(@D)
	@$(CC) $(LDFLAGS) $(OBJS) -o $@

$(OBJ_DIR)/%.o:%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $< -o $@
	@$(CC) $(CFLAGS) -S $< -o $(OBJ_DIR)/$*.lst

PHONY: all clean run