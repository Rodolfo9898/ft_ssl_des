# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#              #
#    Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

include mk/colors.mk
include mk/hint.mk
include mk/config.mk
include mk/libs.mk
include mk/sources.mk

all: $(NAME)

bonus: $(NAME_BONUS)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(NAME)

$(NAME): $(OBJ_FILES) $(LIB_FILES)
	@$(call run,$(CC) $(CFLAGS) $(OBJ_FILES) $(LIB_FILES) -o $@)

$(NAME_BONUS): $(OBJ_BONUS_FILES) $(LIB_BONUS_FILES)
	@$(call run,$(CC) $(CFLAGS) $(OBJ_BONUS_FILES) $(LIB_BONUS_FILES) -o $@)

# The two builds compile the same files into their own object folder, the
# bonus with -D BONUS_MODE=1, so building one never touches the other. The
# objects include the headers of both libraries: fetch the submodules first.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | submodules
	@mkdir -p $(@D)
	@$(call run,$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@)

$(OBJ_BONUS_DIR)/%.o: $(SRC_DIR)/%.c | submodules
	@mkdir -p $(@D)
	@$(call run,$(CC) $(CFLAGS) $(BONUS_FLAGS) $(DEPFLAGS) -c $< -o $@)

# ft_ssl_md5 has libft as a submodule of its own: --recursive fetches it too.
submodules:
	@if [ ! -f $(LIBFT)Makefile ] || [ ! -f $(MD5)Makefile ] \
		|| [ ! -f $(MD5)libs/libft/Makefile ]; then \
		$(call explain,Initializing the submodules in $(LIBS_DIR)); \
		$(call run,git submodule update --init --remote --recursive); \
	fi

# libft is built by its own Makefile, only when one of its files changed.
# submodules is order-only: being phony, it would otherwise make "make -n" show a
# relink on every run, even though a real make does not do one.
$(LIBFT_LIB): $(LIBFT_FILES) | submodules
	@$(call explain,Entering $(LIBFT): building $(notdir $@) with its own Makefile)
	@$(call run,$(MAKE) --no-print-directory -C $(LIBFT))

# ft_ssl_md5 too, then its objects but main.o and dispatch.o go in an archive.
$(MD5_LIB): $(MD5_FILES) | submodules
	@$(call explain,Entering $(MD5): building its objects with its own Makefile)
	@$(call run,$(MAKE) --no-print-directory -C $(MD5) all)
	@mkdir -p $(@D) && rm -f $@
	@$(call run,$(AR) $@ $$(find $(MD5)objects -name \*.o ! \( $(MD5_OWN) \)))

$(MD5_BONUS_LIB): $(MD5_FILES) | submodules
	@$(call explain,Entering $(MD5): building its bonus objects with its own Makefile)
	@$(call run,$(MAKE) --no-print-directory -C $(MD5) bonus)
	@mkdir -p $(@D) && rm -f $@
	@$(call run,$(AR) $@ $$(find $(MD5)objects_bonus -name \*.o ! \( $(MD5_OWN) \)))

clean:
	@$(call remove,$(OBJ_DIR),Removing the objects of the mandatory build)
	@$(call remove,$(OBJ_BONUS_DIR),Removing the objects of the bonus build)
	@if $(call exists,$(LIBFT)objects); then \
		$(call explain,Entering $(LIBFT): removing its objects with its own Makefile); \
		$(call run,$(MAKE) --no-print-directory -sC $(LIBFT) clean); \
	else \
		$(call warn,nothing to clean in $(LIBFT): it has no object files); \
	fi
	@if $(call exists,$(MD5)objects $(MD5)objects_bonus); then \
		$(call explain,Entering $(MD5): removing its objects with its own Makefile); \
		$(call run,$(MAKE) --no-print-directory -sC $(MD5) clean); \
	else \
		$(call warn,nothing to clean in $(MD5): it has no object files); \
	fi

fclean: clean
	@$(call remove,$(NAME),Removing the program $(NAME))
	@$(call remove,$(NAME_BONUS),Removing the program $(NAME_BONUS))
	@if $(call exists,$(LIBFT_LIB)); then \
		$(call explain,Entering $(LIBFT): removing $(notdir $(LIBFT_LIB)) with its own Makefile); \
		$(call run,$(MAKE) --no-print-directory -sC $(LIBFT) fclean); \
	else \
		$(call warn,nothing to remove: $(LIBFT_LIB) does not exist); \
	fi
	@if $(call exists,$(MD5)ft_ssl $(MD5)ft_ssl_bonus $(MD5)libs/libft/libft.a); then \
		$(call explain,Entering $(MD5): removing its programs with its own Makefile); \
		$(call run,$(MAKE) --no-print-directory -sC $(MD5) fclean); \
	else \
		$(call warn,nothing to remove: $(MD5) has built nothing); \
	fi

re: fclean all

# The image copies libs/ as it is: fetch the submodules first. Every rule
# rebuilds the image, so it always tests the current sources (docker's cache
# makes this instant when nothing changed).
docker-build: submodules
	docker build -t $(DOCKER_IMAGE) .

# An interactive shell exits with the status of its last command, which says
# nothing about the rule: only 125, docker itself failing, is an error.
docker-shell: docker-build
	@docker run --rm -it $(DOCKER_IMAGE) bash || [ $$? -ne 125 ]

docker-mandatory: docker-build
	docker run --rm $(DOCKER_IMAGE) bash tests/mandatory.sh

docker-bonus: docker-build
	docker run --rm $(DOCKER_IMAGE) bash tests/bonus.sh

# Removes the test image once you are done with it.
docker-clean:
	@if docker image inspect $(DOCKER_IMAGE) >/dev/null 2>&1; then \
		$(call explain,Removing the docker image $(DOCKER_IMAGE)); \
		$(call run,docker rmi $(DOCKER_IMAGE)); \
	else \
		$(call warn,nothing to remove: there is no docker image $(DOCKER_IMAGE)); \
	fi

.PHONY: all bonus clean debug fclean re submodules \
	docker-build docker-shell docker-mandatory docker-bonus docker-clean

-include $(DEP_FILES)
