/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ofb_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* Only with -D BONUS_MODE=1 (make bonus): see include/core/des_defines.h */
#if BONUS_MODE

/*
** OFB: the IV encrypted again and again gives a stream of bytes, XORed with
** the data. Encrypting and decrypting are the same, and there is no padding:
** the output is as long as the input.
*/
int	des_run_ofb(const t_des_job *job, const t_buffer *in, t_buffer *out)
{
	unsigned char	stream[DES_BLOCK_SIZE];
	uint64_t		state;
	size_t			offset;
	size_t			index;

	state = job->iv;
	offset = 0;
	while (offset < in->len)
	{
		state = job->cipher->crypt_block(state, &job->key, DIRECTION_ENCRYPT);
		des_store_be64(state, stream);
		index = 0;
		while (index < DES_BLOCK_SIZE && offset + index < in->len)
		{
			stream[index] ^= in->data[offset + index];
			index++;
		}
		if (!buffer_append(out, stream, index))
			return (0);
		offset += index;
	}
	return (1);
}

int	run_des_ofb(int argc, char **argv)
{
	static const t_cipher	cipher = {DES_OFB_NAME, DES_MODE_OFB,
		DES_KEY_SIZE, des_crypt_block, des_run_ofb};

	return (run_cipher(&cipher, argc, argv));
}

#endif
