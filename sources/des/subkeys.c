/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   subkeys.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** The 16 round keys of an 8-byte key: PC1 keeps 56 bits in two halves of 28,
** each round turns both left, and PC2 picks 48 bits of the two.
*/
static void	compute_subkeys(const t_des_tables *tables,
	const unsigned char *key, uint64_t *subkeys)
{
	uint64_t	permuted;
	uint32_t	left;
	uint32_t	right;
	int			round;

	permuted = des_permute(des_load_be64(key), tables->pc1, DES_PC1_BITS,
			DES_BLOCK_BITS);
	left = (permuted >> DES_HALF_KEY_BITS) & DES_HALF_KEY_MASK;
	right = permuted & DES_HALF_KEY_MASK;
	round = 0;
	while (round < DES_ROUNDS)
	{
		left = des_rotate28(left, tables->shifts[round]);
		right = des_rotate28(right, tables->shifts[round]);
		subkeys[round] = des_permute(((uint64_t)left << DES_HALF_KEY_BITS)
				| right, tables->pc2, DES_SUBKEY_BITS, DES_PC1_BITS);
		round++;
	}
}

/*
** What the mode needs, from the options: the round keys of each 8-byte key
** (one, or three for Triple DES), the IV as a number, and the direction.
*/
void	des_init_job(t_des_job *job, const t_des_options *opts)
{
	size_t	index;

	ft_bzero(job, sizeof(*job));
	job->cipher = opts->cipher;
	job->direction = opts->direction;
	job->iv = des_load_be64(opts->iv);
	index = 0;
	while (index * DES_KEY_SIZE < opts->cipher->key_size)
	{
		compute_subkeys(des_tables(), opts->key + index * DES_KEY_SIZE,
			job->key.subkeys[index]);
		index++;
	}
}
