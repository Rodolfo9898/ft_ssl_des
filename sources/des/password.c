/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   password.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* "enter des-ecb encryption password:", after prefix ("Verifying - ") */
static void	build_prompt(char *prompt, char *prefix, const t_des_options *opts)
{
	ft_strlcpy(prompt, prefix, PROMPT_SIZE);
	ft_strlcat(prompt, PROMPT_ENTER, PROMPT_SIZE);
	ft_strlcat(prompt, opts->cipher->name, PROMPT_SIZE);
	if (opts->direction == DIRECTION_DECRYPT)
		ft_strlcat(prompt, PROMPT_DECRYPT, PROMPT_SIZE);
	else
		ft_strlcat(prompt, PROMPT_ENCRYPT, PROMPT_SIZE);
}

/*
** A password typed without being shown (getpass, allowed by p.6), copied:
** getpass() keeps it in a buffer of its own, which the next call reuses.
*/
static char	*read_password(char *prompt, char *command)
{
	char	*typed;
	char	*copy;

	typed = getpass(prompt);
	if (typed == NULL)
	{
		cipher_system_error(command, MSG_PASSWORD);
		return (NULL);
	}
	copy = ft_strdup(typed);
	ft_bzero(typed, ft_strlen(typed));
	if (copy == NULL)
		cipher_system_error(command, NULL);
	return (copy);
}

/*
** The password of -p, or one asked on the terminal (p.11). To encrypt it is
** asked twice, like OpenSSL, and both must be the same.
*/
int	des_get_password(t_des_options *opts)
{
	char	prompt[PROMPT_SIZE];
	char	*again;
	int		same;

	if (opts->password != NULL)
		return (1);
	build_prompt(prompt, "", opts);
	opts->password = read_password(prompt, opts->cipher->name);
	if (opts->password == NULL)
		return (0);
	opts->password_owned = 1;
	if (opts->direction == DIRECTION_DECRYPT)
		return (1);
	build_prompt(prompt, PROMPT_VERIFY, opts);
	again = read_password(prompt, opts->cipher->name);
	if (again == NULL)
		return (0);
	same = flag_is(again, opts->password);
	ft_bzero(again, ft_strlen(again));
	free(again);
	if (!same)
		return (cipher_error(opts->cipher->name, MSG_VERIFY_FAILURE));
	return (1);
}
