/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   encode.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* The character of a 6-bit value: A-Z, a-z, 0-9, then '+' and '/' */
static char	sextet_char(uint32_t value)
{
	if (value < BASE64_LETTERS)
		return ('A' + value);
	if (value < BASE64_DIGITS_START)
		return ('a' + value - BASE64_LETTERS);
	if (value < BASE64_PLUS_VALUE)
		return ('0' + value - BASE64_DIGITS_START);
	if (value == BASE64_PLUS_VALUE)
		return (BASE64_PLUS);
	return (BASE64_SLASH);
}

/*
** Up to three bytes (remain) as four characters: the bytes side by side make
** 24 bits, read 6 at a time from the left. A missing byte is a '=' at the end.
*/
static void	encode_group(const unsigned char *src, size_t remain, char *out)
{
	uint32_t	value;
	size_t		index;
	int			shift;

	value = 0;
	index = 0;
	while (index < BASE64_GROUP_SIZE)
	{
		value <<= BYTE_BITS;
		if (index < remain)
			value |= src[index];
		index++;
	}
	index = 0;
	while (index < BASE64_QUAD_SIZE)
	{
		shift = BASE64_SEXTET_BITS * (BASE64_QUAD_SIZE - 1 - index);
		out[index] = BASE64_PAD;
		if (index <= remain)
			out[index] = sextet_char((value >> shift) & BASE64_SEXTET_MASK);
		index++;
	}
}

/*
** data in base64, in lines of 64 characters each ended by a newline, like
** openssl base64 (which other programs then read). Nothing for no data.
*/
int	base64_encode(const unsigned char *data, size_t len, t_buffer *out)
{
	char	chars[BASE64_QUAD_SIZE];
	size_t	index;
	size_t	remain;
	size_t	column;

	index = 0;
	column = 0;
	while (index < len)
	{
		remain = len - index;
		if (remain > BASE64_GROUP_SIZE)
			remain = BASE64_GROUP_SIZE;
		encode_group(data + index, remain, chars);
		if (!buffer_append(out, (unsigned char *)chars, BASE64_QUAD_SIZE))
			return (0);
		column += BASE64_QUAD_SIZE;
		if ((column == BASE64_LINE_WIDTH || index + remain == len)
			&& !buffer_append(out, (unsigned char *)"\n", 1))
			return (0);
		column %= BASE64_LINE_WIDTH;
		index += remain;
	}
	return (1);
}
