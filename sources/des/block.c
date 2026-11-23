/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   block.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** The eight S-boxes, each on 6 of the 48 bits: the first and the last bit
** choose the row, the four in between the column, and the entry gives 4 bits.
*/
static uint32_t	substitute(const t_des_tables *tables, uint64_t expanded)
{
	uint32_t	result;
	uint32_t	chunk;
	uint32_t	row;
	int			box;

	result = 0;
	box = 0;
	while (box < DES_SBOXES)
	{
		chunk = (expanded >> (DES_SBOX_INPUT_BITS * (DES_SBOXES - 1 - box)))
			& DES_SBOX_INPUT_MASK;
		row = ((chunk & DES_SBOX_ROW_HIGH) >> DES_SBOX_ROW_SHIFT)
			| (chunk & DES_SBOX_ROW_LOW);
		result = (result << DES_SBOX_OUTPUT_BITS) | tables->sbox[box][row
			* DES_SBOX_COLUMNS + ((chunk >> 1) & DES_SBOX_COLUMN_MASK)];
		box++;
	}
	return (result);
}

/* The round function f: E, XOR with the round key, the S-boxes, then P */
static uint32_t	feistel(const t_des_tables *tables, uint32_t right,
	uint64_t subkey)
{
	uint64_t	expanded;

	expanded = des_permute(right, tables->e, DES_SUBKEY_BITS, DES_HALF_BITS);
	return (des_permute(substitute(tables, expanded ^ subkey), tables->p,
			DES_HALF_BITS, DES_HALF_BITS));
}

/*
** One block through DES with one key's 16 round keys: IP, 16 rounds that
** each swap the halves, then IP inverted. Decrypting is the same with the
** round keys in the reverse order.
*/
uint64_t	des_crypt_single(uint64_t block, const uint64_t *subkeys,
	t_direction direction)
{
	const t_des_tables	*tables;
	uint64_t			state;
	uint32_t			left;
	uint32_t			right;
	int					round;

	tables = des_tables();
	state = des_permute(block, tables->ip, DES_BLOCK_BITS, DES_BLOCK_BITS);
	left = state >> DES_HALF_BITS;
	right = state & DES_HALF_MASK;
	round = 0;
	while (round < DES_ROUNDS)
	{
		state = right;
		if (direction == DIRECTION_DECRYPT)
			right = left ^ feistel(tables, right,
					subkeys[DES_ROUNDS - 1 - round]);
		else
			right = left ^ feistel(tables, right, subkeys[round]);
		left = state;
		round++;
	}
	state = ((uint64_t)right << DES_HALF_BITS) | left;
	return (des_permute(state, tables->fp, DES_BLOCK_BITS, DES_BLOCK_BITS));
}

/* The block function of the DES commands: one key */
uint64_t	des_crypt_block(uint64_t block, const t_des_key *key,
	t_direction direction)
{
	return (des_crypt_single(block, key->subkeys[DES3_FIRST], direction));
}
