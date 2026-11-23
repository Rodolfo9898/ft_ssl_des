# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    config.mk                                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#              #
#    Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_ssl
NAME_BONUS = ft_ssl_bonus

CC = cc
AR = ar rcs
CFLAGS = -Wall -Wextra -Werror -std=c99 -O2
BONUS_FLAGS = -D BONUS_MODE=1
DEBUG_FLAGS = -g -fsanitize=address
DEPFLAGS = -MMD -MP

SRC_DIR = ./sources
OBJ_DIR = ./objects
OBJ_BONUS_DIR = ./objects_bonus
LIBS_DIR = ./libs
DOCKER_IMAGE = ft_ssl_des-test
