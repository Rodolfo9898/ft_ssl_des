/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   base64_encode.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

static void	encode_triplet(const unsigned char *src, size_t remain,
	unsigned char out[4])
{
	static const char	alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz0123456789+/";
	unsigned char		second;
	unsigned char		third;

	second = 0;
	third = 0;
	if (remain > 1)
		second = src[1];
	if (remain > 2)
		third = src[2];
	out[0] = alphabet[src[0] >> 2];
	out[1] = alphabet[((src[0] & 0x03) << 4) | (second >> 4)];
	out[2] = '=';
	out[3] = '=';
	if (remain > 1)
		out[2] = alphabet[((second & 0x0F) << 2) | (third >> 6)];
	if (remain > 2)
		out[3] = alphabet[third & 0x3F];
}

int	base64_encode_data(const unsigned char *data, size_t len, t_buffer *out)
{
	unsigned char	chunk[4];
	size_t			index;
	size_t			remain;

	index = 0;
	while (index < len)
	{
		remain = len - index;
		if (remain > 3)
			remain = 3;
		encode_triplet(data + index, remain, chunk);
		if (!buffer_append(out, chunk, 4))
			return (0);
		index += remain;
	}
	return (1);
}
