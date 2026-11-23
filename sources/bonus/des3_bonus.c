/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des3_bonus.c                                       :+:      :+:    :+:   */
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
** Triple DES with three keys, E-D-E (p.13): encrypt with the first, decrypt
** with the second, encrypt with the third. Decrypting undoes it backwards.
*/
uint64_t	des3_crypt_block(uint64_t block, const t_des_key *key,
	t_direction direction)
{
	if (direction == DIRECTION_DECRYPT)
	{
		block = des_crypt_single(block, key->subkeys[DES3_THIRD],
				DIRECTION_DECRYPT);
		block = des_crypt_single(block, key->subkeys[DES3_SECOND],
				DIRECTION_ENCRYPT);
		return (des_crypt_single(block, key->subkeys[DES3_FIRST],
				DIRECTION_DECRYPT));
	}
	block = des_crypt_single(block, key->subkeys[DES3_FIRST],
			DIRECTION_ENCRYPT);
	block = des_crypt_single(block, key->subkeys[DES3_SECOND],
			DIRECTION_DECRYPT);
	return (des_crypt_single(block, key->subkeys[DES3_THIRD],
			DIRECTION_ENCRYPT));
}

/* des3 is des3-cbc, as OpenSSL uses CBC for Triple DES by default (p.13) */
int	run_des3(int argc, char **argv)
{
	static const t_cipher	cipher = {DES3_NAME, DES_MODE_CBC, DES3_KEY_SIZE,
		des3_crypt_block, des_run_blocks};

	return (run_cipher(&cipher, argc, argv));
}

int	run_des3_ecb(int argc, char **argv)
{
	static const t_cipher	cipher = {DES3_ECB_NAME, DES_MODE_ECB,
		DES3_KEY_SIZE, des3_crypt_block, des_run_blocks};

	return (run_cipher(&cipher, argc, argv));
}

int	run_des3_cbc(int argc, char **argv)
{
	static const t_cipher	cipher = {DES3_CBC_NAME, DES_MODE_CBC,
		DES3_KEY_SIZE, des3_crypt_block, des_run_blocks};

	return (run_cipher(&cipher, argc, argv));
}

int	run_des3_ofb(int argc, char **argv)
{
	static const t_cipher	cipher = {DES3_OFB_NAME, DES_MODE_OFB,
		DES3_KEY_SIZE, des3_crypt_block, des_run_ofb};

	return (run_cipher(&cipher, argc, argv));
}

#endif
