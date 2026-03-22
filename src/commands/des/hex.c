/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   hex.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

static int	hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (10 + (c - 'a'));
	if (c >= 'A' && c <= 'F')
		return (10 + (c - 'A'));
	return (-1);
}

int	des_parse_hex_bytes(const char *str, unsigned char *out, size_t out_len)
{
	size_t	index;
	size_t	src;
	int		left;
	int		right;

	ft_memset(out, 0, out_len);
	index = 0;
	src = 0;
	while (str[src] != '\0' && index < out_len)
	{
		left = hex_value(str[src++]);
		if (left < 0)
			return (0);
		right = 0;
		if (str[src] != '\0')
			right = hex_value(str[src++]);
		if (right < 0)
			return (0);
		out[index++] = (unsigned char)((left << 4) | right);
	}
	while (str[src] != '\0')
		if (hex_value(str[src++]) < 0)
			return (0);
	return (1);
}

int	des_parse_hex64(const char *value, uint64_t *out)
{
	unsigned char	bytes[8];

	if (!des_parse_hex_bytes(value, bytes, 8))
		return (0);
	*out = des_load_be64(bytes);
	return (1);
}
