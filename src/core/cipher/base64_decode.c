/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_decode.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

static int	decode_char(unsigned char c)
{
	if (c >= 'A' && c <= 'Z')
		return ((int)(c - 'A'));
	if (c >= 'a' && c <= 'z')
		return ((int)(c - 'a') + 26);
	if (c >= '0' && c <= '9')
		return ((int)(c - '0') + 52);
	if (c == '+')
		return (62);
	if (c == '/')
		return (63);
	return (-1);
}

static int	append_decoded_quad(const unsigned char q[4], t_buffer *out)
{
	unsigned char	bytes[3];
	int				v[4];

	if (q[0] == '=' || q[1] == '=')
		return (errno = EINVAL, 0);
	v[0] = decode_char(q[0]);
	v[1] = decode_char(q[1]);
	if (v[0] < 0 || v[1] < 0)
		return (errno = EINVAL, 0);
	bytes[0] = (unsigned char)((v[0] << 2) | (v[1] >> 4));
	if (q[2] == '=' && q[3] == '=')
		return (buffer_append(out, bytes, 1));
	v[2] = decode_char(q[2]);
	if (v[2] < 0)
		return (errno = EINVAL, 0);
	bytes[1] = (unsigned char)(((v[1] & 0x0F) << 4) | (v[2] >> 2));
	if (q[3] == '=')
		return (buffer_append(out, bytes, 2));
	v[3] = decode_char(q[3]);
	if (v[3] < 0)
		return (errno = EINVAL, 0);
	bytes[2] = (unsigned char)(((v[2] & 0x03) << 6) | v[3]);
	return (buffer_append(out, bytes, 3));
}

static int	check_tail_spaces(const unsigned char *data,
		size_t index, size_t len)
{
	while (index < len)
	{
		if ((data[index] == ' ' || data[index] == '\t' || data[index] == '\n'
				|| data[index] == '\r' || data[index] == '\v'
				|| data[index] == '\f') == 0)
			return (errno = EINVAL, 0);
		index += 1;
	}
	return (1);
}

static int	consume_input(const unsigned char *data, size_t len,
		unsigned char quad[4], t_buffer *out)
{
	size_t	index;
	size_t	qi;

	index = 0;
	qi = 0;
	while (index < len)
	{
		if (data[index] == ' ' || data[index] == '\t' || data[index] == '\n'
			|| data[index] == '\r' || data[index] == '\v'
			|| data[index] == '\f')
			index += 1;
		else
			quad[qi++] = data[index++];
		if (qi < 4)
			continue ;
		if (append_decoded_quad(quad, out) == 0)
			return (0);
		if (quad[2] == '=' || quad[3] == '=')
			return (check_tail_spaces(data, index, len));
		qi = 0;
	}
	if (qi > 0)
		return (errno = EINVAL, 0);
	return (1);
}

int	base64_decode_data(const unsigned char *data, size_t len, t_buffer *out)
{
	unsigned char	quad[4];

	return (consume_input(data, len, quad, out));
}
