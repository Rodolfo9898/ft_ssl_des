/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofb.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 21:46:27 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../include/ft_ssl.h"

static void	xor_chunk(unsigned char *dst, const unsigned char *src, size_t len)
{
	size_t	index;

	index = 0;
	while (index < len)
	{
		dst[index] ^= src[index];
		index++;
	}
}

static uint64_t	next_stream(uint64_t state, t_des_options *opts,
	t_des_subkeys *sub)
{
	if (opts->algo == DES_ALGO_3DES)
		return (des3_block_crypt(state, sub, 0));
	return (des_block_crypt(state, sub->k1, 0));
}

static void	init_subkeys(t_des_options *opts, t_des_subkeys *sub)
{
	des_compute_subkeys(opts->keys[0], sub->k1);
	des_compute_subkeys(opts->keys[1], sub->k2);
	des_compute_subkeys(opts->keys[2], sub->k3);
}

int	des_run_ofb(const unsigned char *data, size_t len,
	t_des_options *opts, t_buffer *out)
{
	t_des_subkeys	sub;
	unsigned char	buf[8];
	size_t			index;
	size_t			chunk;
	uint64_t		state;

	init_subkeys(opts, &sub);
	state = opts->iv;
	index = 0;
	while (index < len)
	{
		state = next_stream(state, opts, &sub);
		des_store_be64(state, buf);
		chunk = len - index;
		if (chunk > 8)
			chunk = 8;
		xor_chunk(buf, data + index, chunk);
		if (!buffer_append(out, buf, chunk))
			return (0);
		index += chunk;
	}
	return (1);
}
