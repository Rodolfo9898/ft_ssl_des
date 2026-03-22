/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   io.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/des.h"

int	des_read_random_salt(uint64_t *salt)
{
	int			fd;
	ssize_t		got;
	unsigned char	bytes[8];

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (0);
	got = read(fd, bytes, sizeof(bytes));
	close(fd);
	if (got != (ssize_t)sizeof(bytes))
		return (errno = EIO, 0);
	*salt = des_load_be64(bytes);
	return (1);
}

int	des_load_input(const char *path, t_buffer *buffer)
{
	if (path != NULL)
		return (read_file(path, buffer));
	return (read_stream(STDIN_FILENO, buffer));
}

int	des_write_output(const char *path, const t_buffer *buffer)
{
	int	fd;
	int	ok;

	if (path == NULL)
		return (write_all(STDOUT_FILENO, buffer->data, buffer->len));
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (0);
	ok = write_all(fd, buffer->data, buffer->len);
	close(fd);
	return (ok);
}

int	des_is_salted_header(const t_buffer *input)
{
	if (input->len < 16)
		return (0);
	return (input->data[0] == 'S' && input->data[1] == 'a'
		&& input->data[2] == 'l' && input->data[3] == 't'
		&& input->data[4] == 'e' && input->data[5] == 'd'
		&& input->data[6] == '_' && input->data[7] == '_');
}

int	des_append_salted_header(const t_des_options *opts, t_buffer *buffer)
{
	unsigned char	header[16];

	header[0] = 'S';
	header[1] = 'a';
	header[2] = 'l';
	header[3] = 't';
	header[4] = 'e';
	header[5] = 'd';
	header[6] = '_';
	header[7] = '_';
	des_store_be64(opts->salt, header + 8);
	return (buffer_append(buffer, header, sizeof(header)));
}
