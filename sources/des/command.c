/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* des is an alias for des-cbc, as in OpenSSL (ft_ssl_des.pdf p.7) */
int	run_des(int argc, char **argv)
{
	static const t_cipher	cipher = {DES_NAME, DES_MODE_CBC, DES_KEY_SIZE,
		des_crypt_block, des_run_blocks};

	return (run_cipher(&cipher, argc, argv));
}

/* Each block on its own (p.11) */
int	run_des_ecb(int argc, char **argv)
{
	static const t_cipher	cipher = {DES_ECB_NAME, DES_MODE_ECB,
		DES_KEY_SIZE, des_crypt_block, des_run_blocks};

	return (run_cipher(&cipher, argc, argv));
}

/* Each block chained to the one before it (p.12) */
int	run_des_cbc(int argc, char **argv)
{
	static const t_cipher	cipher = {DES_CBC_NAME, DES_MODE_CBC,
		DES_KEY_SIZE, des_crypt_block, des_run_blocks};

	return (run_cipher(&cipher, argc, argv));
}
