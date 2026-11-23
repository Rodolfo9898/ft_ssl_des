/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   key.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** The key and the IV from the password and the salt, as OpenSSL does: the
** key first, then the IV, unless -k gave the key or -v the IV.
*/
static int	derive_key(t_des_options *opts)
{
	unsigned char	material[DES_MAX_KEY_SIZE + DES_BLOCK_SIZE];
	size_t			key_size;

	key_size = opts->cipher->key_size;
	if (!des_evp_bytes_to_key(opts, material, key_size + DES_BLOCK_SIZE))
		return (cipher_system_error(opts->cipher->name, NULL));
	if (!opts->key_set)
		ft_memcpy(opts->key, material, key_size);
	if (!opts->iv_set)
		ft_memcpy(opts->iv, material + key_size, DES_BLOCK_SIZE);
	opts->key_set = 1;
	opts->iv_set = 1;
	ft_bzero(material, sizeof(material));
	return (1);
}

/*
** The key of the command (p.10-11). With -k alone, the key as given: every
** mode but ECB then needs -v, as openssl -K needs -iv. Otherwise a password
** (-p, or typed) and a salt: when encrypting, -s or a new random one; when
** decrypting, the one in the "Salted__" header (skip is then its size), or
** -s. As in openssl enc, -k with -p still derives from the password, then
** -k replaces the key (and -v the IV). data is the ciphertext when
** decrypting, NULL when encrypting.
*/
int	des_prepare_key(t_des_options *opts, const t_buffer *data, size_t *skip)
{
	if (skip != NULL)
		*skip = 0;
	if (opts->key_set && opts->password == NULL)
	{
		if (opts->cipher->mode != DES_MODE_ECB && !opts->iv_set)
			return (cipher_error(opts->cipher->name, MSG_IV_UNDEFINED));
		return (1);
	}
	if (!des_get_password(opts))
		return (0);
	if (data != NULL && !des_read_header(opts, data, skip))
		return (0);
	if (data == NULL && !opts->salt_set && !des_random_salt(opts))
		return (0);
	return (derive_key(opts));
}
