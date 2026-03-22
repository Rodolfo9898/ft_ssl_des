/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ecb_cbc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/des.h"

static uint64_t	crypt_block(uint64_t block, const t_des_options *opts,
	const t_des_subkeys *sub, int decrypt)
{
	if (opts->algo == DES_ALGO_3DES)
		return (des3_block_crypt(block, sub, decrypt));
	return (des_block_crypt(block, sub->k1, decrypt));
}

static int	strip_padding(t_buffer *out)
{
	size_t	pad;
	size_t	index;

	if (out->len == 0)
		return (errno = EINVAL, 0);
	pad = out->data[out->len - 1];
	if (pad == 0 || pad > 8 || pad > out->len)
		return (errno = EINVAL, 0);
	index = 0;
	while (index < pad)
	{
		if (out->data[out->len - 1 - index] != pad)
			return (errno = EINVAL, 0);
		index++;
	}
	out->len -= pad;
	return (1);
}

static int	run_decrypt_blocks(const t_buffer *in, t_des_options *opts,
	const t_des_subkeys *sub, t_buffer *out)
{
	unsigned char	buf[8];
	uint64_t		block;
	uint64_t		prev;
	size_t			index;

	prev = opts->iv;
	index = 0;
	while (index < in->len)
	{
		ft_memcpy(buf, in->data + index, 8);
		block = crypt_block(des_load_be64(buf), opts, sub, 1);
		if (opts->mode == DES_MODE_CBC)
			block ^= prev;
		if (opts->mode == DES_MODE_CBC)
			prev = des_load_be64(buf);
		des_store_be64(block, buf);
		if (!buffer_append(out, buf, 8))
			return (0);
		index += 8;
	}
	return (strip_padding(out));
}

static int	run_encrypt_blocks(const t_buffer *in, t_des_options *opts,
	const t_des_subkeys *sub, t_buffer *out)
{
	unsigned char	buf[8];
	uint64_t		block;
	uint64_t		prev;
	size_t			index;
	size_t			pad;

	prev = opts->iv;
	index = 0;
	while (index + 8 <= in->len)
	{
		ft_memcpy(buf, in->data + index, 8);
		block = des_load_be64(buf);
		if (opts->mode == DES_MODE_CBC)
			block ^= prev;
		block = crypt_block(block, opts, sub, 0);
		if (opts->mode == DES_MODE_CBC)
			prev = block;
		des_store_be64(block, buf);
		if (!buffer_append(out, buf, 8))
			return (0);
		index += 8;
	}
	pad = 8 - (in->len - index);
	if (pad == 0)
		pad = 8;
	ft_memset(buf, (int)pad, 8);
	if (index < in->len)
		ft_memcpy(buf, in->data + index, in->len - index);
	block = des_load_be64(buf);
	if (opts->mode == DES_MODE_CBC)
		block ^= prev;
	block = crypt_block(block, opts, sub, 0);
	des_store_be64(block, buf);
	return (buffer_append(out, buf, 8));
}

int	des_run_ecb_cbc(const unsigned char *data, size_t len,
	t_des_options *opts, t_buffer *out)
{
	t_des_subkeys	sub;
	t_buffer		in;

	if (opts->decrypt && (len % 8) != 0)
		return (errno = EINVAL, 0);
	des_compute_subkeys(opts->keys[0], sub.k1);
	des_compute_subkeys(opts->keys[1], sub.k2);
	des_compute_subkeys(opts->keys[2], sub.k3);
	in.data = (unsigned char *)data;
	in.len = len;
	if (opts->decrypt)
		return (run_decrypt_blocks(&in, opts, &sub, out));
	return (run_encrypt_blocks(&in, opts, &sub, out));
}
