/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 23:27:48 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 21:42:05 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

static t_whirlpool_tables	*whirlpool_tables(void)
{
	static t_whirlpool_tables	tables = {
		# include "../../../include/command/whirlpool_sbox_words.inc"
	{{0}},
	{0},
		0
	};

	return (&tables);
}

static uint64_t	build_entry(int x)
{
	uint32_t	s;
	uint32_t	v1;
	uint32_t	v2;
	uint32_t	v4;
	uint64_t	entry;

	s = whirlpool_tables()->sbox_words[x / 2];
	if ((x % 2) == 0)
		s >>= 8;
	else
		s &= 0xFF;
	v1 = s << 1;
	if (v1 >= 0x100)
		v1 ^= 0x11D;
	v2 = v1 << 1;
	if (v2 >= 0x100)
		v2 ^= 0x11D;
	v4 = v2 << 1;
	if (v4 >= 0x100)
		v4 ^= 0x11D;
	entry = ((uint64_t)s << 56) | ((uint64_t)s << 48);
	entry |= ((uint64_t)v2 << 40) | ((uint64_t)s << 32);
	entry |= ((uint64_t)v4 << 24) | ((uint64_t)(v2 ^ s) << 16);
	entry |= ((uint64_t)v1 << 8) | (uint64_t)(v4 ^ s);
	return (entry);
}

void	whirlpool_init_tables(void)
{
	t_whirlpool_tables	*tables;
	int					x;
	int					idx;
	int					round;

	tables = whirlpool_tables();
	if (tables->initialized)
		return ;
	x = -1;
	while (++x < 256)
	{
		tables->c[0][x] = build_entry(x);
		idx = 0;
		while (++idx < 8)
			tables->c[idx][x] = (tables->c[idx - 1][x] >> 8)
				| (tables->c[idx - 1][x] << 56);
	}
	tables->rc[0] = 0;
	round = 0;
	while (++round <= 10)
		whirlpool_init_round_key(round, tables->c, tables->rc);
	tables->initialized = 1;
}

uint64_t	whirlpool_transform(const uint64_t src[8], int index)
{
	t_whirlpool_tables	*tables;
	uint64_t			value;
	int					shift;
	int					table;

	tables = whirlpool_tables();
	value = 0;
	shift = 56;
	table = 0;
	while (table < 8)
	{
		value ^= tables->c[table][(src[(index - table) & 7] >> shift) & 0xFF];
		table++;
		shift -= 8;
	}
	return (value);
}

uint64_t	whirlpool_round_constant(int round)
{
	return (whirlpool_tables()->rc[round]);
}
