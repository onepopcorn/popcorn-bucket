SRC_DIR := ./
BIN := ../../out/$(FILENAME)
OBJ_DIR := ./obj

# Optional: Use it to include files from another example
SHARED_SRCS ?=

LOCAL_SRCS	:= $(shell find $(SRC_DIR) -name '*.c')
SHARED_OBJS	:= $(patsubst %.c, $(OBJ_DIR)/shared/%.o, $(notdir $(SHARED_SRCS)))
OBJS		:= $(patsubst %.c, $(OBJ_DIR)/%.o, $(LOCAL_SRCS)) $(SHARED_OBJS)

SHARED_INC := $(sort $(dir $(SHARED_SRCS)))

# make looks up bare names in borrowed folders
vpath %.c $(SHARED_INC)

CFLAGS	:= -I. $(addprefix -I,$(SHARED_INC)) -c -std=gnu99 -Wall -Werror -pedantic -O3 -fomit-frame-pointer -ffast-math -march=i386 -MMD -MP

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

# compiles shared objects.
$(SHARED_OBJS): $(OBJ_DIR)/shared/%.o:%.c
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) $< -o $@
	@$(CC) $(CFLAGS) -S $< -o $(OBJ_DIR)/shared/$*.lst

-include $(OBJS:.o=.d)

PHONY: all clean run