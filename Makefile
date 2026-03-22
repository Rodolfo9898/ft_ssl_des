NAME = ft_ssl

CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c99

INCLUDES = -Iinclude
OBJDIR = objects
SRC = \
	src/main.c \
	src/core/stdin/tokenize.c \
	src/core/stdin/tokenize_utils.c \
	src/core/stdin/commands.c \
	src/commands/pipeline/command.c \
	src/commands/pipeline/sources.c \
	src/commands/pipeline/string.c \
	src/commands/pipeline/options.c \
	src/commands/pipeline/output.c \
	src/commands/base64/command.c \
	src/commands/des/command.c \
	src/commands/des/tables.c \
	src/commands/des/bits.c \
	src/commands/des/core.c \
	src/commands/des/hex.c \
	src/commands/des/io.c \
	src/commands/des/kdf.c \
	src/commands/des/options.c \
	src/commands/des/ecb_cbc.c \
	src/commands/des/ofb.c \
	src/commands/des/process.c \
	src/commands/md5/command.c \
	src/commands/md5/algo.c \
	src/commands/sha256/command.c \
	src/commands/sha256/algo.c \
	src/commands/whirlpool/command.c \
	src/commands/whirlpool/round.c \
	src/commands/whirlpool/algo.c \
	src/commands/help_command.c \
	src/core/command_registry.c \
	src/core/io.c \
	src/core/cipher/base64_encode.c \
	src/core/cipher/base64_decode.c \
	src/core/output/base.c \
	src/core/output/extra.c \
	src/core/utils.c \
	src/core/memory.c
OBJ = $(patsubst src/%.c,$(OBJDIR)/%.o,$(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJDIR)/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
