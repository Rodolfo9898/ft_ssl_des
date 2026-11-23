/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   decode.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* The 6-bit value of a base64 character, or -1 */
static int	sextet_value(unsigned char c)
{
	if (c >= 'A' && c < 'A' + BASE64_LETTERS)
		return (c - 'A');
	if (c >= 'a' && c < 'a' + BASE64_LETTERS)
		return (c - 'a' + BASE64_LETTERS);
	if (c >= '0' && c < '0' + BASE64_DIGITS)
		return (c - '0' + BASE64_DIGITS_START);
	if (c == BASE64_PLUS)
		return (BASE64_PLUS_VALUE);
	if (c == BASE64_SLASH)
		return (BASE64_SLASH_VALUE);
	return (-1);
}

/*
** The 24 bits of four characters. '=' can only be the last one or two, and
** nothing but '=' after the first one. Returns the number of '=', or -1 when
** the group is not valid base64.
*/
static int	quad_value(const unsigned char *quad, uint32_t *value)
{
	int	pads;
	int	index;
	int	sextet;

	*value = 0;
	pads = 0;
	index = 0;
	while (index < BASE64_QUAD_SIZE)
	{
		sextet = sextet_value(quad[index]);
		if (quad[index] == BASE64_PAD
			&& index >= BASE64_QUAD_SIZE - BASE64_MAX_PADS)
			pads++;
		else if (pads > 0 || sextet < 0)
			return (-1);
		else
			*value |= sextet << (BASE64_SEXTET_BITS
					* (BASE64_QUAD_SIZE - 1 - index));
		index++;
	}
	return (pads);
}

/* Four characters back into up to three bytes: the number of '=', or -1 */
static int	decode_quad(const unsigned char *quad, t_buffer *out)
{
	unsigned char	bytes[BASE64_GROUP_SIZE];
	uint32_t		value;
	int				pads;
	int				index;

	pads = quad_value(quad, &value);
	if (pads < 0)
	{
		cipher_invalid_data();
		return (-1);
	}
	index = 0;
	while (index < BASE64_GROUP_SIZE)
	{
		bytes[index] = value >> (BYTE_BITS * (BASE64_GROUP_SIZE - 1 - index));
		index++;
	}
	if (!buffer_append(out, bytes, BASE64_GROUP_SIZE - pads))
		return (-1);
	return (pads);
}

/*
** Adds a character to the group: when it has four, decodes them. Returns the
** number of '=' of the group (0 while it is not full), or -1.
*/
static int	feed_char(unsigned char *quad, size_t *filled, unsigned char c,
	t_buffer *out)
{
	quad[*filled] = c;
	(*filled)++;
	if (*filled < BASE64_QUAD_SIZE)
		return (0);
	*filled = 0;
	return (decode_quad(quad, out));
}

/*
** base64 back into bytes, skipping the whitespace ("d G9 0bwo\n=", p.9).
** Nothing but whitespace may follow the group with '='. 0 with errno at
** EINVAL when it is not base64 (a character out of the set, a group cut
** short, something after the '='), at ENOMEM when malloc failed.
*/
int	base64_decode(const unsigned char *data, size_t len, t_buffer *out)
{
	unsigned char	quad[BASE64_QUAD_SIZE];
	size_t			index;
	size_t			filled;
	int				pads;

	index = 0;
	filled = 0;
	pads = 0;
	while (index < len)
	{
		if (!is_space(data[index]) && pads > 0)
			return (cipher_invalid_data());
		if (!is_space(data[index]))
			pads = feed_char(quad, &filled, data[index], out);
		if (pads < 0)
			return (0);
		index++;
	}
	if (filled != 0)
		return (cipher_invalid_data());
	return (1);
}
