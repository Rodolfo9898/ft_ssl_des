/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   salt.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** A new salt, 8 random bytes (p.11: it changes every time, man 4 random).
** The ciphertext then starts with "Salted__" and the salt, as OpenSSL's does,
** so that decrypting finds it again.
*/
int	des_random_salt(t_des_options *opts)
{
	int		fd;
	ssize_t	got;

	fd = open(RANDOM_DEVICE, O_RDONLY);
	if (fd < 0)
		return (cipher_system_error(opts->cipher->name, RANDOM_DEVICE));
	got = read(fd, opts->salt, SALT_SIZE);
	close(fd);
	if (got != SALT_SIZE)
	{
		if (got >= 0)
			errno = EIO;
		return (cipher_system_error(opts->cipher->name, RANDOM_DEVICE));
	}
	opts->salt_set = 1;
	opts->salted_header = 1;
	return (1);
}

/* "Salted__", then the 8 bytes of the salt */
int	des_append_header(const t_des_options *opts, t_buffer *out)
{
	return (buffer_append(out, (const unsigned char *)SALT_MAGIC,
			SALT_MAGIC_SIZE)
		&& buffer_append(out, opts->salt, SALT_SIZE));
}

/*
** Decrypting with a password: the salt of the "Salted__" header, skipped
** (skip is its size), unless -s gives another. Without the header, -s must
** give it: else "bad magic number", like OpenSSL.
*/
int	des_read_header(t_des_options *opts, const t_buffer *data, size_t *skip)
{
	*skip = 0;
	if (data->len >= SALT_HEADER_SIZE
		&& ft_memcmp(data->data, SALT_MAGIC, SALT_MAGIC_SIZE) == 0)
	{
		*skip = SALT_HEADER_SIZE;
		if (!opts->salt_set)
			ft_memcpy(opts->salt, data->data + SALT_MAGIC_SIZE, SALT_SIZE);
		opts->salt_set = 1;
	}
	if (!opts->salt_set)
		return (cipher_error(opts->cipher->name, MSG_BAD_MAGIC));
	return (1);
}
