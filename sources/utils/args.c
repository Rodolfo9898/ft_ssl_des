/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   args.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* Whether arg is exactly flag (or a command name exactly another) */
int	flag_is(const char *arg, const char *flag)
{
	return (ft_strncmp(arg, flag, ft_strlen(flag) + 1) == 0);
}

/* Spaces, tabs and newlines, which base64 skips */
int	is_space(unsigned char c)
{
	return (c != '\0' && ft_strchr(WHITESPACE, c) != NULL);
}

/* The value of a hexadecimal digit, either case, or -1 */
static int	hex_digit(char c)
{
	char	*found;

	if (c == '\0')
		return (-1);
	found = ft_strchr(HEX_DIGITS, ft_tolower(c));
	if (found == NULL)
		return (-1);
	return (found - HEX_DIGITS);
}

/*
** A hex string into out_len bytes, like openssl -K, -S and -iv: a short one
** is padded with zeros on the right (FF1 is FF10...), a long one is cut. 0
** when it is empty or has a character that is not a hex digit.
*/
int	hex_parse(const char *str, unsigned char *out, size_t out_len)
{
	size_t	index;
	int		digit;

	if (str[0] == '\0')
		return (0);
	ft_bzero(out, out_len);
	index = 0;
	while (str[index] != '\0')
	{
		digit = hex_digit(str[index]);
		if (digit < 0)
			return (0);
		if (index / HEX_PER_BYTE < out_len && index % HEX_PER_BYTE == 0)
			out[index / HEX_PER_BYTE] = digit * HEX_BASE;
		else if (index / HEX_PER_BYTE < out_len)
			out[index / HEX_PER_BYTE] += digit;
		index++;
	}
	return (1);
}
