/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kdf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/des.h"

int	des_evp_bytes_to_material(const char *password, uint64_t salt,
	unsigned char *material, size_t need_len)
{
	unsigned char	digest[16];
	unsigned char	salt_bytes[8];
	unsigned char	buffer[1024];
	size_t			pass_len;
	size_t			state[3];

	pass_len = ft_strlen(password);
	if (pass_len + 24 > sizeof(buffer))
		return (errno = EINVAL, 0);
	des_store_be64(salt, salt_bytes);
	state[0] = 0;
	state[1] = 0;
	state[2] = 16;
	while (state[0] < need_len)
	{
		if (state[1] > 0)
			ft_memcpy(buffer, digest, state[1]);
		if (pass_len > 0)
			ft_memcpy(buffer + state[1], (const unsigned char *)password, pass_len);
		ft_memcpy(buffer + state[1] + pass_len, salt_bytes, 8);
		if (!get_md5_algorithm()->compute(buffer, state[1] + pass_len + 8, digest))
			return (0);
		if (state[0] + state[2] > need_len)
			state[2] = need_len - state[0];
		ft_memcpy(material + state[0], digest, state[2]);
		state[0] += state[2];
		state[1] = 16;
	}
	return (1);
}

static int	set_password(t_des_options *opts)
{
	char	*prompt;

	if (opts->pass_set)
		return (1);
	if (opts->decrypt)
		prompt = getpass("enter des decryption password:");
	else
		prompt = getpass("enter des encryption password:");
	if (prompt == NULL)
		return (0);
	opts->password = prompt;
	opts->pass_set = 1;
	return (1);
}

static void	set_keys_and_iv(t_des_options *opts, const unsigned char *material)
{
	opts->keys[0] = des_load_be64(material);
	if (opts->algo == DES_ALGO_3DES)
	{
		opts->keys[1] = des_load_be64(material + 8);
		opts->keys[2] = des_load_be64(material + 16);
		if (!opts->iv_set)
			opts->iv = des_load_be64(material + 24);
	}
	else if (!opts->iv_set)
		opts->iv = des_load_be64(material + 8);
}

static int	extract_header_salt(t_des_options *opts, t_buffer *input)
{
	if (!opts->decrypt || !des_is_salted_header(input))
		return (1);
	if (!opts->salt_set)
	{
		opts->salt = des_load_be64(input->data + 8);
		opts->salt_set = 1;
	}
	ft_memcpy(input->data, input->data + 16, input->len - 16);
	input->len -= 16;
	return (1);
}

int	des_prepare_key_material(t_des_options *opts, t_buffer *input,
	int *derived_from_password)
{
	unsigned char	material[32];
	size_t			need_len;

	*derived_from_password = 0;
	if (opts->key_set)
		return (1);
	if (!set_password(opts) || !extract_header_salt(opts, input))
		return (0);
	if (!opts->decrypt && !opts->salt_set)
		if (!des_read_random_salt(&opts->salt))
			return (0);
	if (!opts->salt_set)
		opts->salt = 0;
	need_len = 16;
	if (opts->algo == DES_ALGO_3DES)
		need_len = 32;
	if (!des_evp_bytes_to_material(opts->password, opts->salt,
			material, need_len))
		return (0);
	set_keys_and_iv(opts, material);
	opts->key_set = 1;
	*derived_from_password = 1;
	return (1);
}
