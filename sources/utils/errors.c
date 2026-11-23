/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   errors.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** The errors of the cipher commands, on the standard error. Each returns 0,
** the failure of the function that calls it: "return (cipher_error(...))".
*/

/* "ft_ssl: command: message" */
int	cipher_error(char *command, char *message)
{
	ft_putstr_fd("ft_ssl: ", STDERR_FILENO);
	ft_putstr_fd(command, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putendl_fd(message, STDERR_FILENO);
	return (0);
}

/* "ft_ssl: command: message 'arg'": what was wrong, and with what */
int	cipher_arg_error(char *command, char *message, char *arg)
{
	ft_putstr_fd("ft_ssl: ", STDERR_FILENO);
	ft_putstr_fd(command, STDERR_FILENO);
	ft_putstr_fd(": ", STDERR_FILENO);
	ft_putstr_fd(message, STDERR_FILENO);
	ft_putstr_fd(" '", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd("'", STDERR_FILENO);
	return (0);
}

/* A bad flag: the error, then "usage: ft_ssl command usage" */
int	cipher_usage_error(char *command, char *message, char *arg, char *usage)
{
	cipher_arg_error(command, message, arg);
	ft_putstr_fd(MSG_USAGE_START, STDERR_FILENO);
	ft_putstr_fd(command, STDERR_FILENO);
	ft_putstr_fd(" ", STDERR_FILENO);
	ft_putendl_fd(usage, STDERR_FILENO);
	return (0);
}

/* A system call or malloc failed: ft_ssl_md5's message, from errno */
int	cipher_system_error(char *command, char *target)
{
	print_system_error(command, target);
	return (0);
}

/*
** Data that cannot be what it should (base64 that is not, a ciphertext that
** does not decrypt): errno at EINVAL, which the caller tells from ENOMEM.
*/
int	cipher_invalid_data(void)
{
	errno = EINVAL;
	return (0);
}
