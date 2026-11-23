# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    sources.mk                                         :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#              #
#    Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# main.c alone at the root (the only main() of both builds), then one level
# of folders with files only: utils/ the command table, the errors, the flags
# and the files, base64/ the base64 command, des/ DES, its modes and its key
# from a password, bonus/ the bonus part. md5 and sha256 are ft_ssl_md5's.
MAIN_SRCS = main.c
UTILS_DIR = utils
BASE64_DIR = base64
DES_DIR = des
BONUS_DIR = bonus

UTILS_SRCS = args.c cipher_io.c dispatch.c errors.c
BASE64_SRCS = command.c decode.c encode.c
DES_SRCS = bits.c block.c command.c kdf.c key.c modes.c options.c \
			password.c run.c salt.c subkeys.c tables.c

# The bonus part. Both builds compile every file: make bonus adds
# -D BONUS_MODE=1 (BONUS_FLAGS), without which these files are empty and the
# command table has neither des-ofb nor Triple DES, see include/core/des_defines.h.
BONUS_SRCS = des3_bonus.c ofb_bonus.c

SRC_FILES = $(MAIN_SRCS) \
			$(addprefix $(UTILS_DIR)/,$(UTILS_SRCS)) \
			$(addprefix $(BASE64_DIR)/,$(BASE64_SRCS)) \
			$(addprefix $(DES_DIR)/,$(DES_SRCS)) \
			$(addprefix $(BONUS_DIR)/,$(BONUS_SRCS))

OBJ_FILES = $(addprefix $(OBJ_DIR)/,$(SRC_FILES:.c=.o))
OBJ_BONUS_FILES = $(addprefix $(OBJ_BONUS_DIR)/,$(SRC_FILES:.c=.o))
DEP_FILES = $(OBJ_FILES:.o=.d) $(OBJ_BONUS_FILES:.o=.d)
