/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   core.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

void	des_compute_subkeys(uint64_t key, uint64_t subkeys[16])
{
	uint64_t	pc1;
	uint32_t	c;
	uint32_t	d;
	int			round;

	pc1 = des_permute(key, g_des_pc1, 56, 64);
	c = (uint32_t)((pc1 >> 28) & 0x0FFFFFFF);
	d = (uint32_t)(pc1 & 0x0FFFFFFF);
	round = 0;
	while (round < 16)
	{
		c = des_left_rotate28(c, g_des_shifts[round]);
		d = des_left_rotate28(d, g_des_shifts[round]);
		subkeys[round] = des_permute((((uint64_t)c) << 28) | d,
				g_des_pc2, 48, 56);
		round++;
	}
}

static uint32_t	des_feistel_lookup(uint64_t x, int index)
{
	uint32_t	chunk;
	uint32_t	row;
	uint32_t	col;

	chunk = (uint32_t)((x >> (42 - (index * 6))) & 0x3F);
	row = ((chunk & 0x20) >> 4) | (chunk & 0x01);
	col = (chunk >> 1) & 0x0F;
	return ((uint32_t)g_des_sbox[index][row * 16 + col]);
}

static uint32_t	des_feistel(uint32_t right, uint64_t subkey)
{
	uint64_t	x;
	uint32_t	s_out;
	int			index;

	x = des_permute((uint64_t)right, g_des_e, 48, 32) ^ subkey;
	s_out = 0;
	index = 0;
	while (index < 8)
	{
		s_out <<= 4;
		s_out |= des_feistel_lookup(x, index);
		index++;
	}
	return ((uint32_t)des_permute((uint64_t)s_out, g_des_p, 32, 32));
}

uint64_t	des_block_crypt(uint64_t block, const uint64_t subkeys[16],
	int decrypt)
{
	uint64_t	ip;
	uint32_t	left;
	uint32_t	right;
	uint32_t	next;
	int			round;

	ip = des_permute(block, g_des_ip, 64, 64);
	left = (uint32_t)(ip >> 32);
	right = (uint32_t)(ip & 0xFFFFFFFF);
	round = 0;
	while (round < 16)
	{
		next = right;
		if (decrypt)
			right = left ^ des_feistel(right, subkeys[15 - round]);
		else
			right = left ^ des_feistel(right, subkeys[round]);
		left = next;
		round++;
	}
	return (des_permute((((uint64_t)right) << 32) | left, g_des_fp, 64, 64));
}

uint64_t	des3_block_crypt(uint64_t block, const t_des_subkeys *keys,
	int decrypt)
{
	if (decrypt)
	{
		block = des_block_crypt(block, keys->k3, 1);
		block = des_block_crypt(block, keys->k2, 0);
		return (des_block_crypt(block, keys->k1, 1));
	}
	block = des_block_crypt(block, keys->k1, 0);
	block = des_block_crypt(block, keys->k2, 1);
	return (des_block_crypt(block, keys->k3, 0));
}
