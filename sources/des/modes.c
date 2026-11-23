/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   modes.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** The padding of OpenSSL (PKCS#7): the last byte is how many bytes were
** added, from 1 to 8, all with that value. Anything else does not decrypt.
*/
static int	strip_padding(t_buffer *out)
{
	size_t	pad;
	size_t	index;

	if (out->len == 0)
		return (cipher_invalid_data());
	pad = out->data[out->len - 1];
	if (pad == 0 || pad > DES_BLOCK_SIZE || pad > out->len)
		return (cipher_invalid_data());
	index = 0;
	while (index < pad)
	{
		if (out->data[out->len - 1 - index] != pad)
			return (cipher_invalid_data());
		index++;
	}
	out->len -= pad;
	return (1);
}

/*
** One block encrypted. In CBC it is first XORed with the block encrypted
** before it (the IV for the first one): chain then holds this one.
*/
static uint64_t	encrypt_one(const t_des_job *job, uint64_t block,
	uint64_t *chain)
{
	if (job->cipher->mode == DES_MODE_CBC)
		block ^= *chain;
	block = job->cipher->crypt_block(block, &job->key, DIRECTION_ENCRYPT);
	*chain = block;
	return (block);
}

/*
** Every full block, then a last one padded: n bytes short of 8 are n bytes of
** value n, and a length that is a multiple of 8 gets a whole block of 8.
*/
static int	encrypt_blocks(const t_des_job *job, const t_buffer *in,
	t_buffer *out)
{
	unsigned char	block[DES_BLOCK_SIZE];
	uint64_t		chain;
	size_t			offset;
	size_t			rest;

	chain = job->iv;
	offset = 0;
	while (offset <= in->len)
	{
		rest = in->len - offset;
		if (rest > DES_BLOCK_SIZE)
			rest = DES_BLOCK_SIZE;
		ft_memset(block, DES_BLOCK_SIZE - rest, DES_BLOCK_SIZE);
		ft_memcpy(block, in->data + offset, rest);
		des_store_be64(encrypt_one(job, des_load_be64(block), &chain), block);
		if (!buffer_append(out, block, DES_BLOCK_SIZE))
			return (0);
		offset += DES_BLOCK_SIZE;
	}
	return (1);
}

/*
** Each block decrypted, then in CBC XORed with the ciphertext block before
** it (the IV for the first one). The length must be a multiple of 8, and the
** padding right: else EINVAL, which the command prints as "bad decrypt".
*/
static int	decrypt_blocks(const t_des_job *job, const t_buffer *in,
	t_buffer *out)
{
	unsigned char	block[DES_BLOCK_SIZE];
	uint64_t		chain;
	uint64_t		ciphertext;
	uint64_t		plaintext;
	size_t			offset;

	if (in->len == 0 || in->len % DES_BLOCK_SIZE != 0)
		return (cipher_invalid_data());
	chain = job->iv;
	offset = 0;
	while (offset < in->len)
	{
		ciphertext = des_load_be64(in->data + offset);
		plaintext = job->cipher->crypt_block(ciphertext, &job->key,
				DIRECTION_DECRYPT);
		if (job->cipher->mode == DES_MODE_CBC)
			plaintext ^= chain;
		chain = ciphertext;
		des_store_be64(plaintext, block);
		if (!buffer_append(out, block, DES_BLOCK_SIZE))
			return (0);
		offset += DES_BLOCK_SIZE;
	}
	return (strip_padding(out));
}

/* ECB and CBC: the mode of des, des-ecb, des-cbc and their Triple DES */
int	des_run_blocks(const t_des_job *job, const t_buffer *in, t_buffer *out)
{
	if (job->direction == DIRECTION_DECRYPT)
		return (decrypt_blocks(job, in, out));
	return (encrypt_blocks(job, in, out));
}
