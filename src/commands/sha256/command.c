/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 23:27:35 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 21:42:05 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

static const t_sha256_tables	*sha256_tables(void)
{
	static const t_sha256_tables	tables = {
		# include "../../../include/command/sha256_k.inc"
	};

	return (&tables);
}

static uint32_t	right_rotate(uint32_t value, uint32_t shift)
{
	return ((value >> shift) | (value << (32 - shift)));
}

static void	build_schedule(uint32_t words[64], const unsigned char *block)
{
	int			index;
	uint32_t	tmp[2];

	index = -1;
	while (++index < 16)
	{
		words[index] = ((uint32_t)block[index * 4] << 24);
		words[index] |= ((uint32_t)block[index * 4 + 1] << 16);
		words[index] |= ((uint32_t)block[index * 4 + 2] << 8);
		words[index] |= (uint32_t)block[index * 4 + 3];
	}
	while (index < 64)
	{
		tmp[0] = right_rotate(words[index - 15], 7);
		tmp[0] ^= right_rotate(words[index - 15], 18);
		tmp[0] ^= words[index - 15] >> 3;
		tmp[1] = right_rotate(words[index - 2], 17);
		tmp[1] ^= right_rotate(words[index - 2], 19);
		tmp[1] ^= words[index - 2] >> 10;
		words[index] = words[index - 16] + tmp[0];
		words[index] += words[index - 7] + tmp[1];
		index++;
	}
}

static void	round_step(uint32_t state[8], uint32_t word, int index)
{
	const t_sha256_tables	*tables;
	uint32_t				tmp[2];
	uint32_t				maj;
	uint32_t				sigma1;

	tables = sha256_tables();
	sigma1 = right_rotate(state[4], 6) ^ right_rotate(state[4], 11);
	sigma1 ^= right_rotate(state[4], 25);
	tmp[0] = state[7];
	tmp[0] += sigma1;
	tmp[0] += (state[4] & state[5]) ^ ((~state[4]) & state[6]);
	tmp[0] += tables->k[index] + word;
	maj = (state[0] & state[1]) ^ (state[0] & state[2]);
	maj ^= state[1] & state[2];
	tmp[1] = right_rotate(state[0], 2) ^ right_rotate(state[0], 13);
	tmp[1] ^= right_rotate(state[0], 22);
	tmp[1] += maj;
	state[7] = state[6];
	state[6] = state[5];
	state[5] = state[4];
	state[4] = state[3] + tmp[0];
	state[3] = state[2];
	state[2] = state[1];
	state[1] = state[0];
	state[0] = tmp[0] + tmp[1];
}

void	sha256_process_block(t_sha256_ctx *ctx, const unsigned char *block)
{
	uint32_t	words[64];
	uint32_t	state[8];
	int			index;

	build_schedule(words, block);
	index = -1;
	while (++index < 8)
		state[index] = ctx->h[index];
	index = -1;
	while (++index < 64)
		round_step(state, words[index], index);
	index = -1;
	while (++index < 8)
		ctx->h[index] += state[index];
}
