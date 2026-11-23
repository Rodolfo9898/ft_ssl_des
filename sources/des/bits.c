/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bits.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* Eight bytes as a number, the first byte the most significant (big-endian) */
uint64_t	des_load_be64(const unsigned char *bytes)
{
	uint64_t	value;
	int			index;

	value = 0;
	index = 0;
	while (index < DES_BLOCK_SIZE)
	{
		value = (value << BYTE_BITS) | bytes[index];
		index++;
	}
	return (value);
}

/* The other way: a number into eight bytes, most significant first */
void	des_store_be64(uint64_t value, unsigned char *bytes)
{
	int	index;

	index = DES_BLOCK_SIZE - 1;
	while (index >= 0)
	{
		bytes[index] = value % BYTE_RANGE;
		value >>= BYTE_BITS;
		index--;
	}
}

/*
** The permutations of FIPS 46-3: bit i of the out_bits of the result is bit
** table[i] of the in_bits of src, counted from 1 at the most significant.
*/
uint64_t	des_permute(uint64_t src, const int *table, int out_bits,
	int in_bits)
{
	uint64_t	out;
	int			index;

	out = 0;
	index = 0;
	while (index < out_bits)
	{
		out = (out << 1) | ((src >> (in_bits - table[index])) & 1);
		index++;
	}
	return (out);
}

/* A key half turned shift places left on its 28 bits */
uint32_t	des_rotate28(uint32_t value, int shift)
{
	return (((value << shift) | (value >> (DES_HALF_KEY_BITS - shift)))
		& DES_HALF_KEY_MASK);
}
