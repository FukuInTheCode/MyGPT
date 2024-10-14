NAME   = mygpt

CC	   = gcc

WFLAGS = -Wall -Wextra

LIBS = -lm -lc -lcurl

CFLAGS = -I./include/ $(WFLAGS) $(LIBS)

SRC	= $(shell find src/ -type f -name "*.c")

BUILD_DIR = ./build/
BUILD_OBJ_DIR = ./build/obj
BUILD_CP_DIR = ./build/mirror

OBJ	= $(SRC:src/%.c=$(BUILD_OBJ_DIR)/%.o)
MIRROR_SRC	= $(SRC:src/%.c=$(BUILD_CP_DIR)/%.c)

RED = \033[1;31m

GREEN = \033[1;32m

BLUE = \033[1;34m

NC = \033[0m

all: $(NAME)

$(NAME): $(MIRROR_SRC) $(OBJ)
	@echo -e "$(BLUE)Compiling binary...$(NC)"
	@$(CC) -o $(NAME) $(OBJ) $(CFLAGS)

$(BUILD_CP_DIR)/%.c: src/%.c
	@echo -e "$(GREEN)Copying $<...$(NC)"
	@mkdir -p $(dir $@)
	@cp $< $@
	@sed -i 's/ malloc(/ __alloc_malloc(/g' $@
	@sed -i 's/ free(/ __alloc_free(/g' $@

$(BUILD_OBJ_DIR)/%.o: $(BUILD_CP_DIR)/%.c
	@echo -e "$(GREEN)Compiling $<...$(NC)"
	@mkdir -p $(dir $@)
	@$(CC) -c -o $@ $< $(CFLAGS)

clean:
	@if [ -d $(BUILD_DIR) ]; then echo -e "$(RED)Cleaning objects.$(NC)"; fi
	@rm -rf $(BUILD_DIR)

fclean: clean
	@if [ -e "$(NAME)" ]; then echo -e "$(RED)Cleaning binary.$(NC)"; fi
	@rm -f $(NAME)

tests_run:
	@echo -e "$(BLUE)Running tests...$(NC)"

d: debug

debug: CFLAGS += -g
debug: fclean $(NAME)
	@valgrind --log-file=/tmp/valgrind-$(NAME) --track-fds=all \
		--leak-check=full --show-leak-kinds=all ./$(NAME)
	@cat /tmp/valgrind-$(NAME)

re: fclean all

