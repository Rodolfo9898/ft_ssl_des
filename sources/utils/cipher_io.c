/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cipher_io.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* All of -i's file, or of the standard input without -i: an error if not */
int	cipher_read_input(char *command, char *path, t_buffer *in)
{
	if (path != NULL && read_file(path, in))
		return (1);
	if (path == NULL && read_stream(STDIN_FILENO, in))
		return (1);
	if (path == NULL)
		return (cipher_system_error(command, MSG_STDIN));
	return (cipher_system_error(command, path));
}

/*
** The result into -o's file (created, or emptied first), or on the standard
** output without -o. A failed write or close (a full disk) is an error.
*/
int	cipher_write_output(char *command, char *path, const t_buffer *out)
{
	int	fd;
	int	ok;

	if (path == NULL && write_all(STDOUT_FILENO, out->data, out->len))
		return (1);
	if (path == NULL)
		return (cipher_system_error(command, MSG_STDOUT));
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, OUTPUT_FILE_MODE);
	if (fd < 0)
		return (cipher_system_error(command, path));
	ok = write_all(fd, out->data, out->len);
	if (close(fd) < 0)
		ok = 0;
	if (!ok)
		return (cipher_system_error(command, path));
	return (1);
}
