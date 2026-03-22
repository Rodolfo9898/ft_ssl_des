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

#include "ft_ssl.h"

static size_t	build_input(unsigned char *buf, unsigned char *prev,
	size_t prev_len, const char *password)
{
	size_t	pass_len;
	size_t	len;

	pass_len = ft_strlen(password);
	len = 0;
	if (prev_len > 0)
	{
		ft_memcpy(buf, prev, prev_len);
		len += prev_len;
	}
	if (pass_len > 0)
	{
		ft_memcpy(buf + len, (const unsigned char *)password, pass_len);
		len += pass_len;
	}
	return (len);
}

int	des_evp_bytes_to_material(const char *password, uint64_t salt,
	unsigned char *material, size_t need_len)
{
	unsigned char	digest[32];
	unsigned char	salt_bytes[8];
	unsigned char	buffer[1024];
	size_t			generated;
	size_t			copy_len;

	if (ft_strlen(password) + 40 > sizeof(buffer))
		return (errno = EINVAL, 0);
	des_store_be64(salt, salt_bytes);
	generated = 0;
	copy_len = 0;
	while (generated < need_len)
	{
		copy_len = build_input(buffer, digest, copy_len, password);
		ft_memcpy(buffer + copy_len, salt_bytes, 8);
		if (!get_sha256_algorithm()->compute(buffer, copy_len + 8, digest))
			return (0);
		copy_len = 32;
		if (generated + copy_len > need_len)
			copy_len = need_len - generated;
		ft_memcpy(material + generated, digest, copy_len);
		generated += copy_len;
		copy_len = 32;
	}
	return (1);
}

static int	prepare_password_and_salt(t_des_options *opts, t_buffer *input)
{
	char	*prompt;

	if (!opts->pass_set)
	{
		if (opts->decrypt)
			prompt = getpass("enter des decryption password:");
		else
			prompt = getpass("enter des encryption password:");
		if (prompt == NULL)
			return (0);
		opts->password = prompt;
		opts->pass_set = 1;
	}
	if (opts->decrypt && des_is_salted_header(input))
	{
		if (!opts->salt_set)
		{
			opts->salt = des_load_be64(input->data + 8);
			opts->salt_set = 1;
		}
		ft_memcpy(input->data, input->data + 16, input->len - 16);
		input->len -= 16;
	}
	return (1);
}

static void	apply_material(t_des_options *opts, const unsigned char *material)
{
	opts->keys[0] = des_load_be64(material);
	opts->keys[1] = 0;
	opts->keys[2] = 0;
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

int	des_prepare_key_material(t_des_options *opts, t_buffer *input,
	int *derived_from_password)
{
	unsigned char	material[32];
	size_t			need_len;

	*derived_from_password = 0;
	if (opts->key_set)
		return (1);
	if (!prepare_password_and_salt(opts, input))
		return (0);
	if (!opts->decrypt && !opts->salt_set && !des_read_random_salt(&opts->salt))
		return (0);
	if (!opts->salt_set)
		opts->salt = 0;
	need_len = 16;
	if (opts->algo == DES_ALGO_3DES)
		need_len = 32;
	if (!des_evp_bytes_to_material(opts->password, opts->salt,
			material, need_len))
		return (0);
	apply_material(opts, material);
	opts->key_set = 1;
	*derived_from_password = 1;
	return (1);
}
