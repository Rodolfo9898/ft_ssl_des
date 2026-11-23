# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    libs.mk                                            :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#              #
#    Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# libft: the string, memory and output helpers, built by its own Makefile.
LIBFT = $(LIBS_DIR)/libft/
LIBFT_LIB = $(addprefix $(LIBFT),libft.a)
LIBFT_FILES = $(wildcard $(LIBFT)Makefile $(LIBFT)mk/*.mk \
	$(LIBFT)include/*.h $(LIBFT)include/core/*.h $(LIBFT)sources/*.c $(LIBFT)sources/*/*.c)

# ft_ssl_md5: the previous project, which this one builds upon (ft_ssl_des.pdf
# p.7): md5, sha256, the buffers, the output and, in its bonus, whirlpool and
# the commands read from the standard input. Its own Makefile compiles it (make,
# or make bonus for the bonus objects); its objects then go in an archive,
# without its main() and its command table: ft_ssl_des has its own, with the
# cipher commands. It brings its own copy of libft as a submodule, for its
# headers: the link uses the one of libs/libft.
MD5 = $(LIBS_DIR)/ft_ssl_md5/
MD5_FILES = $(wildcard $(MD5)Makefile $(MD5)mk/*.mk \
	$(MD5)include/*.h $(MD5)include/core/*.h $(MD5)sources/*.c $(MD5)sources/*/*.c)
MD5_OWN = -name main.o -o -name dispatch.o
MD5_LIB = $(OBJ_DIR)/libft_ssl_md5.a
MD5_BONUS_LIB = $(OBJ_BONUS_DIR)/libft_ssl_md5.a

# ft_ssl_md5 before libft: it uses libft.
LIB_FILES = $(MD5_LIB) $(LIBFT_LIB)
LIB_BONUS_FILES = $(MD5_BONUS_LIB) $(LIBFT_LIB)
