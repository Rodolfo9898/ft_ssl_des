/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bits.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/des.h"

uint64_t	des_load_be64(const unsigned char *bytes)
{
	uint64_t	value;
	int			index;

	value = 0;
	index = 0;
	while (index < 8)
	{
		value = (value << 8) | bytes[index];
		index++;
	}
	return (value);
}

void	des_store_be64(uint64_t value, unsigned char *bytes)
{
	int	index;

	index = 7;
	while (index >= 0)
	{
		bytes[index] = (unsigned char)(value & 0xFF);
		value >>= 8;
		index--;
	}
}

uint64_t	des_permute(uint64_t src, const int *table, int out_len, int in_len)
{
	uint64_t	out;
	int			index;

	out = 0;
	index = 0;
	while (index < out_len)
	{
		out <<= 1;
		out |= (src >> (in_len - table[index])) & 1ULL;
		index++;
	}
	return (out);
}

uint32_t	des_left_rotate28(uint32_t value, int shift)
{
	return (((value << shift) | (value >> (28 - shift))) & 0x0FFFFFFF);
}
