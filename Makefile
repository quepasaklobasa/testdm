NAME = minishell
CC = gcc
CFLAGS = -Wall -Wextra -Werror -Iinclude -g
LDFLAGS = -lreadline
LIBFT = libft/libft.a

SRC_DIR = src
OBJ_DIR = obj

SRCS = \
	$(SRC_DIR)/execution/execution.c \
	$(SRC_DIR)/execution/external.c \
	$(SRC_DIR)/execution/external_utils.c \
	$(SRC_DIR)/execution/utils.c \
	$(SRC_DIR)/builtins/builtin.c \
	$(SRC_DIR)/builtins/builtin_utils.c \
	$(SRC_DIR)/shell/shell.c \
	$(SRC_DIR)/lexer/lexer.c \
	$(SRC_DIR)/lexer/lexer_utils.c \
	$(SRC_DIR)/lexer/lexer_operators.c \
	$(SRC_DIR)/lexer/lexer_helper.c \
	$(SRC_DIR)/lexer/lexer_super_word.c \
	$(SRC_DIR)/parser/parser.c \
	$(SRC_DIR)/parser/command.c \
	$(SRC_DIR)/parser/command_list.c \
	$(SRC_DIR)/main/main.c

OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT)
	$(CC) $(OBJS) $(LIBFT) $(LDFLAGS) -o $(NAME)

$(LIBFT):
	make -C libft

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c inc/minishell.h
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	make -C libft clean

fclean: clean
	rm -f $(NAME)
	make -C libft fclean

re: fclean all

.PHONY: all clean fclean re
