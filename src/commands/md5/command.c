/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 23:27:27 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 21:42:05 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../include/ft_ssl.h"

static const t_md5_tables	*md5_tables(void)
{
	static const t_md5_tables	tables = {
		# include "../../../include/command/md5_tables.inc"
	};

	return (&tables);
}

static uint32_t	left_rotate(uint32_t value, uint32_t shift)
{
	return ((value << shift) | (value >> (32 - shift)));
}

static uint32_t	read_u32_le(const unsigned char *src)
{
	return ((uint32_t)src[0] | ((uint32_t)src[1] << 8)
		| ((uint32_t)src[2] << 16) | ((uint32_t)src[3] << 24));
}

static void	round_step(int index, uint32_t state[4], const uint32_t words[16])
{
	const t_md5_tables	*tables;
	uint32_t			f;
	uint32_t			g;

	tables = md5_tables();
	if (index < 16)
		f = (state[1] & state[2]) | ((~state[1]) & state[3]);
	else if (index < 32)
		f = (state[3] & state[1]) | ((~state[3]) & state[2]);
	else if (index < 48)
		f = state[1] ^ state[2] ^ state[3];
	else
		f = state[2] ^ (state[1] | (~state[3]));
	g = index;
	if (index >= 16 && index < 32)
		g = (uint32_t)((5 * index + 1) % 16);
	else if (index >= 32 && index < 48)
		g = (uint32_t)((3 * index + 5) % 16);
	else if (index >= 48)
		g = (uint32_t)((7 * index) % 16);
	f += state[0] + tables->k[index] + words[g];
	state[0] = state[3];
	state[3] = state[2];
	state[2] = state[1];
	state[1] += left_rotate(f, tables->s[index]);
}

void	md5_process_block(t_md5_ctx *ctx, const unsigned char *block)
{
	uint32_t	words[16];
	uint32_t	state[4];
	int			index;

	index = -1;
	while (++index < 16)
		words[index] = read_u32_le(block + (index * 4));
	state[0] = ctx->a;
	state[1] = ctx->b;
	state[2] = ctx->c;
	state[3] = ctx->d;
	index = -1;
	while (++index < 64)
		round_step(index, state, words);
	ctx->a += state[0];
	ctx->b += state[1];
	ctx->c += state[2];
	ctx->d += state[3];
}
