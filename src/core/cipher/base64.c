/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64.c                                           :+:      :+:    :+:   */
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

int	base64_encode_data(const unsigned char *data, size_t len, t_buffer *out)
{
	static const char	alphabet[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char		chunk[4];
	size_t				index;
	size_t				remain;

	index = 0;
	while (index < len)
	{
		remain = len - index;
		chunk[0] = alphabet[data[index] >> 2];
		chunk[1] = alphabet[((data[index] & 0x03) << 4)
			| ((remain > 1 ? data[index + 1] : 0) >> 4)];
		chunk[2] = '=';
		chunk[3] = '=';
		if (remain > 1)
			chunk[2] = alphabet[((data[index + 1] & 0x0F) << 2)
				| ((remain > 2 ? data[index + 2] : 0) >> 6)];
		if (remain > 2)
			chunk[3] = alphabet[data[index + 2] & 0x3F];
		if (!buffer_append(out, chunk, 4))
			return (0);
		if (remain > 2)
			index += 3;
		else
			index = len;
	}
	return (1);
}

static int	append_decoded_quad(const unsigned char q[4], t_buffer *out)
{
	unsigned char	bytes[3];
	int				v0;
	int				v1;
	int				v2;
	int				v3;

	if (q[0] == '=' || q[1] == '=')
		return (errno = EINVAL, 0);
	v0 = decode_char(q[0]);
	v1 = decode_char(q[1]);
	if (v0 < 0 || v1 < 0)
		return (errno = EINVAL, 0);
	if (q[2] == '=' && q[3] == '=')
	{
		bytes[0] = (unsigned char)((v0 << 2) | (v1 >> 4));
		return (buffer_append(out, bytes, 1));
	}
	v2 = decode_char(q[2]);
	if (v2 < 0)
		return (errno = EINVAL, 0);
	if (q[3] == '=')
	{
		bytes[0] = (unsigned char)((v0 << 2) | (v1 >> 4));
		bytes[1] = (unsigned char)(((v1 & 0x0F) << 4) | (v2 >> 2));
		return (buffer_append(out, bytes, 2));
	}
	v3 = decode_char(q[3]);
	if (v3 < 0)
		return (errno = EINVAL, 0);
	bytes[0] = (unsigned char)((v0 << 2) | (v1 >> 4));
	bytes[1] = (unsigned char)(((v1 & 0x0F) << 4) | (v2 >> 2));
	bytes[2] = (unsigned char)(((v2 & 0x03) << 6) | v3);
	return (buffer_append(out, bytes, 3));
}

int	base64_decode_data(const unsigned char *data, size_t len, t_buffer *out)
{
	unsigned char	quad[4];
	size_t			qi;
	size_t			index;
	int				seen_padding;

	qi = 0;
	index = 0;
	seen_padding = 0;
	while (index < len)
	{
		if (data[index] == ' ' || data[index] == '\t'
			|| data[index] == '\n' || data[index] == '\r'
			|| data[index] == '\v' || data[index] == '\f')
		{
			index++;
			continue ;
		}
		if (seen_padding)
			return (errno = EINVAL, 0);
		quad[qi++] = data[index++];
		if (qi == 4)
		{
			if (!append_decoded_quad(quad, out))
				return (0);
			if (quad[2] == '=' || quad[3] == '=')
				seen_padding = 1;
			qi = 0;
		}
	}
	if (qi != 0)
		return (errno = EINVAL, 0);
	return (1);
}
