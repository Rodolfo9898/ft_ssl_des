/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   output_extra.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 23:06:10 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/15 23:06:11 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

int	write_all(int fd, const void *data, size_t len)
{
	const unsigned char	*bytes;
	ssize_t				written;

	bytes = (const unsigned char *)data;
	while (len > 0)
	{
		written = write(fd, bytes, len);
		if (written < 0)
			return (0);
		bytes += written;
		len -= (size_t)written;
	}
	return (1);
}

static void	print_group(t_command_group group, const char *title)
{
	size_t			count;
	size_t			index;
	const t_command	*commands;

	commands = get_command_table(&count);
	ft_putendl_fd(2, title);
	index = 0;
	while (index < count)
	{
		if (commands[index].group == group)
			ft_putendl_fd(2, commands[index].name);
		index++;
	}
}

void	print_command_list(void)
{
	print_group(COMMAND_GROUP_STANDARD, "Standard commands:");
	print_group(COMMAND_GROUP_MESSAGE_DIGEST, "Message Digest commands:");
	print_group(COMMAND_GROUP_CIPHER, "Cipher commands:");
}

void	print_system_error(const char *command, const char *target)
{
	ft_putstr_fd(2, "ft_ssl: ");
	if (command != NULL && command[0] != '\0')
	{
		ft_putstr_fd(2, command);
		ft_putstr_fd(2, ": ");
	}
	if (target != NULL && target[0] != '\0')
	{
		ft_putstr_fd(2, target);
		ft_putstr_fd(2, ": ");
	}
	ft_putendl_fd(2, strerror(errno));
}
