/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

typedef enum e_des_mode
{
	DES_MODE_ECB,
	DES_MODE_CBC,
	DES_MODE_OFB
}t_des_mode;

typedef enum e_des_algo
{
	DES_ALGO_DES,
	DES_ALGO_3DES
}t_des_algo;

typedef struct s_des_options
{
	t_des_algo	algo;
	t_des_mode	mode;
	int			decrypt;
	int			base64;
	char		*input_path;
	char		*output_path;
	int			key_set;
	uint64_t	keys[3];
	int			pass_set;
	char		*password;
	int			salt_set;
	uint64_t	salt;
	int			iv_set;
	uint64_t	iv;
}t_des_options;

static const int	g_ip[64] = {
	58, 50, 42, 34, 26, 18, 10, 2,
	60, 52, 44, 36, 28, 20, 12, 4,
	62, 54, 46, 38, 30, 22, 14, 6,
	64, 56, 48, 40, 32, 24, 16, 8,
	57, 49, 41, 33, 25, 17, 9, 1,
	59, 51, 43, 35, 27, 19, 11, 3,
	61, 53, 45, 37, 29, 21, 13, 5,
	63, 55, 47, 39, 31, 23, 15, 7
};

static const int	g_fp[64] = {
	40, 8, 48, 16, 56, 24, 64, 32,
	39, 7, 47, 15, 55, 23, 63, 31,
	38, 6, 46, 14, 54, 22, 62, 30,
	37, 5, 45, 13, 53, 21, 61, 29,
	36, 4, 44, 12, 52, 20, 60, 28,
	35, 3, 43, 11, 51, 19, 59, 27,
	34, 2, 42, 10, 50, 18, 58, 26,
	33, 1, 41, 9, 49, 17, 57, 25
};

static const int	g_e[48] = {
	32, 1, 2, 3, 4, 5,
	4, 5, 6, 7, 8, 9,
	8, 9, 10, 11, 12, 13,
	12, 13, 14, 15, 16, 17,
	16, 17, 18, 19, 20, 21,
	20, 21, 22, 23, 24, 25,
	24, 25, 26, 27, 28, 29,
	28, 29, 30, 31, 32, 1
};

static const int	g_p[32] = {
	16, 7, 20, 21, 29, 12, 28, 17,
	1, 15, 23, 26, 5, 18, 31, 10,
	2, 8, 24, 14, 32, 27, 3, 9,
	19, 13, 30, 6, 22, 11, 4, 25
};

static const int	g_pc1[56] = {
	57, 49, 41, 33, 25, 17, 9,
	1, 58, 50, 42, 34, 26, 18,
	10, 2, 59, 51, 43, 35, 27,
	19, 11, 3, 60, 52, 44, 36,
	63, 55, 47, 39, 31, 23, 15,
	7, 62, 54, 46, 38, 30, 22,
	14, 6, 61, 53, 45, 37, 29,
	21, 13, 5, 28, 20, 12, 4
};

static const int	g_pc2[48] = {
	14, 17, 11, 24, 1, 5,
	3, 28, 15, 6, 21, 10,
	23, 19, 12, 4, 26, 8,
	16, 7, 27, 20, 13, 2,
	41, 52, 31, 37, 47, 55,
	30, 40, 51, 45, 33, 48,
	44, 49, 39, 56, 34, 53,
	46, 42, 50, 36, 29, 32
};

static const int	g_shifts[16] = {
	1, 1, 2, 2, 2, 2, 2, 2,
	1, 2, 2, 2, 2, 2, 2, 1
};

static const int	g_sbox[8][64] = {
	{
		14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
		0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
		4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
		15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13
	},
	{
		15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
		3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
		0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
		13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9
	},
	{
		10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
		13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
		13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
		1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12
	},
	{
		7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
		13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
		10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
		3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14
	},
	{
		2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
		14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
		4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
		11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3
	},
	{
		12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
		10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
		9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
		4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13
	},
	{
		4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
		13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
		1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
		6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12
	},
	{
		13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
		1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
		7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
		2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11
	}
};

static uint64_t	load_be64(const unsigned char *bytes)
{
	uint64_t	value;
	int			index;

	value = 0;
	index = 0;
	while (index < 8)
	{
		value = (value << 8) | bytes[index];
		index++;
	}
	return (value);
}

static void	store_be64(uint64_t value, unsigned char *bytes)
{
	int	index;

	index = 7;
	while (index >= 0)
	{
		bytes[index] = (unsigned char)(value & 0xFF);
		value >>= 8;
		index--;
	}
}

static uint64_t	permute(uint64_t src, const int *table, int out_len, int in_len)
{
	uint64_t	out;
	int			index;

	out = 0;
	index = 0;
	while (index < out_len)
	{
		out <<= 1;
		out |= (src >> (in_len - table[index])) & 1ULL;
		index++;
	}
	return (out);
}

static uint32_t	left_rotate28(uint32_t v, int shift)
{
	return (((v << shift) | (v >> (28 - shift))) & 0x0FFFFFFF);
}

static void	compute_subkeys(uint64_t key, uint64_t subkeys[16])
{
	uint64_t	pc1;
	uint32_t	c;
	uint32_t	d;
	uint64_t	cd;
	int			round;

	pc1 = permute(key, g_pc1, 56, 64);
	c = (uint32_t)((pc1 >> 28) & 0x0FFFFFFF);
	d = (uint32_t)(pc1 & 0x0FFFFFFF);
	round = 0;
	while (round < 16)
	{
		c = left_rotate28(c, g_shifts[round]);
		d = left_rotate28(d, g_shifts[round]);
		cd = (((uint64_t)c) << 28) | d;
		subkeys[round] = permute(cd, g_pc2, 48, 56);
		round++;
	}
}

static uint32_t	feistel(uint32_t right, uint64_t subkey)
{
	uint64_t	expanded;
	uint64_t	x;
	uint32_t	out;
	uint32_t	s_out;
	int			index;
	int			six;

	expanded = permute((uint64_t)right, g_e, 48, 32);
	x = expanded ^ subkey;
	s_out = 0;
	index = 0;
	while (index < 8)
	{
		six = (int)((x >> (42 - 6 * index)) & 0x3F);
		s_out <<= 4;
		s_out |= (uint32_t)g_sbox[index][((((six & 0x20) >> 4) | (six & 1)) * 16)
			+ ((six >> 1) & 0x0F)];
		index++;
	}
	out = (uint32_t)permute((uint64_t)s_out, g_p, 32, 32);
	return (out);
}

static uint64_t	des_block(uint64_t block, const uint64_t subkeys[16], int decrypt)
{
	uint64_t	ip;
	uint32_t	left;
	uint32_t	right;
	uint32_t	next_left;
	int			round;

	ip = permute(block, g_ip, 64, 64);
	left = (uint32_t)(ip >> 32);
	right = (uint32_t)(ip & 0xFFFFFFFF);
	round = 0;
	while (round < 16)
	{
		next_left = right;
		if (decrypt)
			right = left ^ feistel(right, subkeys[15 - round]);
		else
			right = left ^ feistel(right, subkeys[round]);
		left = next_left;
		round++;
	}
	return (permute((((uint64_t)right) << 32) | left, g_fp, 64, 64));
}

static uint64_t	des3_block(uint64_t block, const uint64_t k1[16],
	const uint64_t k2[16], const uint64_t k3[16], int decrypt)
{
	uint64_t	v;

	v = block;
	if (!decrypt)
	{
		v = des_block(v, k1, 0);
		v = des_block(v, k2, 1);
		v = des_block(v, k3, 0);
	}
	else
	{
		v = des_block(v, k3, 1);
		v = des_block(v, k2, 0);
		v = des_block(v, k1, 1);
	}
	return (v);
}

static int	hex_value(char c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	if (c >= 'a' && c <= 'f')
		return (10 + (c - 'a'));
	if (c >= 'A' && c <= 'F')
		return (10 + (c - 'A'));
	return (-1);
}

static int	parse_hex_bytes(const char *str, unsigned char *out, size_t out_len)
{
	int			left;
	int			right;
	size_t		index;
	size_t		src;

	ft_memset(out, 0, out_len);
	index = 0;
	src = 0;
	while (str[src] != '\0' && index < out_len)
	{
		left = hex_value(str[src]);
		if (left < 0)
			return (0);
		right = 0;
		if (str[src + 1] != '\0')
		{
			right = hex_value(str[src + 1]);
			if (right < 0)
				return (0);
			src += 2;
		}
		else
			src += 1;
		out[index++] = (unsigned char)((left << 4) | right);
	}
	while (str[src] != '\0')
	{
		if (hex_value(str[src]) < 0)
			return (0);
		src++;
	}
	return (1);
}

static int	read_random_salt(uint64_t *salt)
{
	int			fd;
	unsigned char	bytes[8];
	ssize_t			got;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return (0);
	got = read(fd, bytes, sizeof(bytes));
	close(fd);
	if (got != (ssize_t)sizeof(bytes))
		return (errno = EIO, 0);
	*salt = load_be64(bytes);
	return (1);
}

static int	evp_bytes_to_material(const char *password, uint64_t salt,
	unsigned char *material, size_t need_len)
{
	unsigned char	digest[16];
	unsigned char	prev[16];
	unsigned char	buffer[1024];
	unsigned char	salt_bytes[8];
	size_t			pass_len;
	size_t			generated;
	size_t			copy;
	size_t			input_len;

	pass_len = ft_strlen(password);
	if (pass_len + 16 + 8 > sizeof(buffer))
		return (errno = EINVAL, 0);
	store_be64(salt, salt_bytes);
	generated = 0;
	copy = 0;
	while (generated < need_len)
	{
		input_len = 0;
		if (copy > 0)
		{
			ft_memcpy(buffer, prev, copy);
			input_len += copy;
		}
		if (pass_len > 0)
		{
			ft_memcpy(buffer + input_len, (const unsigned char *)password, pass_len);
			input_len += pass_len;
		}
		ft_memcpy(buffer + input_len, salt_bytes, 8);
		input_len += 8;
		if (!get_md5_algorithm()->compute(buffer, input_len, digest))
			return (0);
		copy = sizeof(digest);
		if (generated + copy > need_len)
			copy = need_len - generated;
		ft_memcpy(material + generated, digest, copy);
		ft_memcpy(prev, digest, sizeof(digest));
		generated += copy;
		copy = sizeof(digest);
	}
	return (1);
}

static int	load_input(const char *path, t_buffer *buffer)
{
	if (path != NULL)
		return (read_file(path, buffer));
	return (read_stream(STDIN_FILENO, buffer));
}

static int	write_output(const char *path, const t_buffer *buffer)
{
	int	fd;
	int	ok;

	if (path == NULL)
		return (write_all(STDOUT_FILENO, buffer->data, buffer->len));
	fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (0);
	ok = write_all(fd, buffer->data, buffer->len);
	close(fd);
	return (ok);
}

static int	run_ecb_or_cbc(const unsigned char *data, size_t len,
	t_des_options *opts, t_buffer *out, int decrypt)
{
	unsigned char	block[8];
	unsigned char	res[8];
	uint64_t		k1[16];
	uint64_t		k2[16];
	uint64_t		k3[16];
	uint64_t		prev;
	uint64_t		v;
	size_t			index;
	size_t			pad;
	int				i;

	compute_subkeys(opts->keys[0], k1);
	compute_subkeys(opts->keys[1], k2);
	compute_subkeys(opts->keys[2], k3);
	prev = opts->iv;
	if (decrypt)
	{
		if ((len % 8) != 0)
			return (errno = EINVAL, 0);
		index = 0;
		while (index < len)
		{
			ft_memcpy(block, data + index, 8);
			v = load_be64(block);
			if (opts->algo == DES_ALGO_3DES)
				v = des3_block(v, k1, k2, k3, 1);
			else
				v = des_block(v, k1, 1);
			if (opts->mode == DES_MODE_CBC)
				v ^= prev;
			if (opts->mode == DES_MODE_CBC)
				prev = load_be64(block);
			store_be64(v, res);
			if (!buffer_append(out, res, 8))
				return (0);
			index += 8;
		}
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
	index = 0;
	while (index + 8 <= len)
	{
		ft_memcpy(block, data + index, 8);
		v = load_be64(block);
		if (opts->mode == DES_MODE_CBC)
			v ^= prev;
		if (opts->algo == DES_ALGO_3DES)
			v = des3_block(v, k1, k2, k3, 0);
		else
			v = des_block(v, k1, 0);
		if (opts->mode == DES_MODE_CBC)
			prev = v;
		store_be64(v, res);
		if (!buffer_append(out, res, 8))
			return (0);
		index += 8;
	}
	pad = 8 - (len % 8);
	if (pad == 0)
		pad = 8;
	i = 0;
	while (i < 8)
	{
		if (index + (size_t)i < len)
			block[i] = data[index + i];
		else
			block[i] = (unsigned char)pad;
		i++;
	}
	v = load_be64(block);
	if (opts->mode == DES_MODE_CBC)
		v ^= prev;
	if (opts->algo == DES_ALGO_3DES)
		v = des3_block(v, k1, k2, k3, 0);
	else
		v = des_block(v, k1, 0);
	store_be64(v, res);
	return (buffer_append(out, res, 8));
}

static int	run_ofb(const unsigned char *data, size_t len,
	t_des_options *opts, t_buffer *out)
{
	uint64_t	k1[16];
	uint64_t	k2[16];
	uint64_t	k3[16];
	uint64_t	stream;
	uint64_t	v;
	unsigned char	block[8];
	unsigned char	res[8];
	size_t		index;
	size_t		chunk;

	compute_subkeys(opts->keys[0], k1);
	compute_subkeys(opts->keys[1], k2);
	compute_subkeys(opts->keys[2], k3);
	stream = opts->iv;
	index = 0;
	while (index < len)
	{
		if (opts->algo == DES_ALGO_3DES)
			stream = des3_block(stream, k1, k2, k3, 0);
		else
			stream = des_block(stream, k1, 0);
		store_be64(stream, block);
		chunk = len - index;
		if (chunk > 8)
			chunk = 8;
		ft_memcpy(res, data + index, chunk);
		v = 0;
		while (v < chunk)
		{
			res[v] ^= block[v];
			v++;
		}
		if (!buffer_append(out, res, chunk))
			return (0);
		index += chunk;
	}
	return (1);
}

static int	parse_k_value(t_des_options *opts, const char *value)
{
	unsigned char	bytes[24];

	if (opts->algo == DES_ALGO_3DES)
	{
		if (!parse_hex_bytes(value, bytes, 24))
			return (0);
		opts->keys[0] = load_be64(bytes);
		opts->keys[1] = load_be64(bytes + 8);
		opts->keys[2] = load_be64(bytes + 16);
	}
	else
	{
		if (!parse_hex_bytes(value, bytes, 8))
			return (0);
		opts->keys[0] = load_be64(bytes);
		opts->keys[1] = 0;
		opts->keys[2] = 0;
	}
	opts->key_set = 1;
	return (1);
}

static int	parse_hex64_value(const char *value, uint64_t *out)
{
	unsigned char	bytes[8];

	if (!parse_hex_bytes(value, bytes, 8))
		return (0);
	*out = load_be64(bytes);
	return (1);
}

static void	init_defaults(t_des_options *opts, const t_command *command)
{
	opts->algo = DES_ALGO_DES;
	opts->mode = DES_MODE_CBC;
	if (ft_is_streq(command->name, "des-ecb"))
		opts->mode = DES_MODE_ECB;
	if (ft_is_streq(command->name, "des-ofb"))
		opts->mode = DES_MODE_OFB;
	if (ft_is_streq(command->name, "des3") || ft_is_streq(command->name, "des3-cbc")
		|| ft_is_streq(command->name, "des3-ecb") || ft_is_streq(command->name, "des3-ofb"))
		opts->algo = DES_ALGO_3DES;
	if (ft_is_streq(command->name, "des3-ecb"))
		opts->mode = DES_MODE_ECB;
	if (ft_is_streq(command->name, "des3-ofb"))
		opts->mode = DES_MODE_OFB;
	opts->decrypt = 0;
	opts->base64 = 0;
	opts->input_path = NULL;
	opts->output_path = NULL;
	opts->key_set = 0;
	opts->keys[0] = 0;
	opts->keys[1] = 0;
	opts->keys[2] = 0;
	opts->pass_set = 0;
	opts->password = NULL;
	opts->salt_set = 0;
	opts->salt = 0;
	opts->iv_set = 0;
	opts->iv = 0;
}

static int	parse_option_with_arg(t_des_options *opts, int argc,
	char **argv, int *index)
{
	const char	*flag;
	const char	*value;
	int			ok;

	flag = argv[*index];
	if (*index + 1 >= argc)
		return (0);
	value = argv[*index + 1];
	ok = 1;
	if (ft_is_streq(flag, "-i"))
		opts->input_path = (char *)value;
	else if (ft_is_streq(flag, "-o"))
		opts->output_path = (char *)value;
	else if (ft_is_streq(flag, "-k"))
		ok = parse_k_value(opts, value);
	else if (ft_is_streq(flag, "-p"))
	{
		opts->pass_set = 1;
		opts->password = (char *)value;
	}
	else if (ft_is_streq(flag, "-s"))
	{
		ok = parse_hex64_value(value, &opts->salt);
		opts->salt_set = ok;
	}
	else if (ft_is_streq(flag, "-v"))
	{
		ok = parse_hex64_value(value, &opts->iv);
		opts->iv_set = ok;
	}
	else
		ok = 0;
	*index += 1;
	return (ok);
}

static int	parse_options(t_des_options *opts, int argc, char **argv)
{
	int	index;

	index = 0;
	while (index < argc)
	{
		if (ft_is_streq(argv[index], "-a"))
			opts->base64 = 1;
		else if (ft_is_streq(argv[index], "-d"))
			opts->decrypt = 1;
		else if (ft_is_streq(argv[index], "-e"))
			opts->decrypt = 0;
		else if (!parse_option_with_arg(opts, argc, argv, &index))
			return (0);
		index++;
	}
	return (1);
}

static int	is_salted_header(const t_buffer *input)
{
	if (input->len < 16)
		return (0);
	return (input->data[0] == 'S' && input->data[1] == 'a' && input->data[2] == 'l'
		&& input->data[3] == 't' && input->data[4] == 'e' && input->data[5] == 'd'
		&& input->data[6] == '_' && input->data[7] == '_');
}

static int	prepare_key_material(t_des_options *opts, t_buffer *input,
	int *derived_from_password)
{
	unsigned char	material[32];
	size_t			need_len;
	char			*prompt;

	*derived_from_password = 0;
	if (opts->key_set)
		return (1);
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
	if (!opts->decrypt && !opts->salt_set)
	{
		if (!read_random_salt(&opts->salt))
			return (0);
		opts->salt_set = 1;
	}
	if (opts->decrypt && is_salted_header(input))
	{
		if (!opts->salt_set)
		{
			opts->salt = load_be64(input->data + 8);
			opts->salt_set = 1;
		}
		ft_memcpy(input->data, input->data + 16, input->len - 16);
		input->len -= 16;
	}
	if (!opts->salt_set)
		opts->salt = 0;
	need_len = 16;
	if (opts->algo == DES_ALGO_3DES)
		need_len = 32;
	if (!evp_bytes_to_material(opts->password, opts->salt, material, need_len))
		return (0);
	opts->keys[0] = load_be64(material);
	if (opts->algo == DES_ALGO_3DES)
	{
		opts->keys[1] = load_be64(material + 8);
		opts->keys[2] = load_be64(material + 16);
		if (!opts->iv_set)
			opts->iv = load_be64(material + 24);
	}
	else
	{
		opts->keys[1] = 0;
		opts->keys[2] = 0;
		if (!opts->iv_set)
			opts->iv = load_be64(material + 8);
	}
	opts->key_set = 1;
	*derived_from_password = 1;
	return (1);
}

static int	prepare_input_for_decrypt(t_des_options *opts, t_buffer *input,
	t_buffer *working)
{
	if (!opts->base64)
		return (buffer_append(working, input->data, input->len));
	return (base64_decode_data(input->data, input->len, working));
}

static int	append_salted_header(const t_des_options *opts, t_buffer *buffer)
{
	unsigned char	header[16];

	header[0] = 'S';
	header[1] = 'a';
	header[2] = 'l';
	header[3] = 't';
	header[4] = 'e';
	header[5] = 'd';
	header[6] = '_';
	header[7] = '_';
	store_be64(opts->salt, header + 8);
	return (buffer_append(buffer, header, sizeof(header)));
}

static int	wrap_encrypted_output(t_des_options *opts, t_buffer *raw,
	int derived_from_password, t_buffer *output)
{
	t_buffer	wrapped;

	wrapped.data = NULL;
	wrapped.len = 0;
	wrapped.capacity = 0;
	if (derived_from_password)
	{
		if (!append_salted_header(opts, &wrapped))
			return (buffer_free(&wrapped), 0);
	}
	if (!buffer_append(&wrapped, raw->data, raw->len))
		return (buffer_free(&wrapped), 0);
	if (!opts->base64)
		return (buffer_append(output, wrapped.data, wrapped.len), buffer_free(&wrapped), 1);
	if (!base64_encode_data(wrapped.data, wrapped.len, output))
		return (buffer_free(&wrapped), 0);
	if (!buffer_append(output, (const unsigned char *)"\n", 1))
		return (buffer_free(&wrapped), 0);
	buffer_free(&wrapped);
	return (1);
}

static int	run_cipher_data(const unsigned char *data, size_t len,
	t_des_options *opts, t_buffer *out, int decrypt)
{
	if (opts->mode == DES_MODE_OFB)
		return (run_ofb(data, len, opts, out));
	return (run_ecb_or_cbc(data, len, opts, out, decrypt));
}

int	run_des_command(const t_command *command, int argc, char **argv)
{
	t_des_options	opts;
	t_buffer		input;
	t_buffer		decoded;
	t_buffer		raw;
	int				derived_from_password;
	int				ok;

	init_defaults(&opts, command);
	input.data = NULL;
	input.len = 0;
	input.capacity = 0;
	decoded.data = NULL;
	decoded.len = 0;
	decoded.capacity = 0;
	raw.data = NULL;
	raw.len = 0;
	raw.capacity = 0;
	if (!parse_options(&opts, argc, argv))
		return (ft_putendl_fd(2, "ft_ssl: des: invalid options"), 1);
	if (!load_input(opts.input_path, &input))
		return (print_system_error(command->name,
			opts.input_path ? opts.input_path : "stdin"), 1);
	if (!opts.iv_set)
		opts.iv = 0;
	if (!opts.decrypt)
	{
		if (!prepare_key_material(&opts, &input, &derived_from_password)
			|| !run_cipher_data(input.data, input.len, &opts, &raw, 0)
			|| !wrap_encrypted_output(&opts, &raw, derived_from_password, &decoded))
			ok = 0;
		else
			ok = 1;
	}
	else
	{
		if (!prepare_input_for_decrypt(&opts, &input, &decoded)
			|| !prepare_key_material(&opts, &decoded, &derived_from_password)
			|| !run_cipher_data(decoded.data, decoded.len, &opts, &raw, 1))
			ok = 0;
		else
			ok = 1;
	}
	if (!ok || !write_output(opts.output_path, opts.decrypt ? &raw : &decoded))
	{
		print_system_error(command->name, ok ?
			(opts.output_path ? opts.output_path : "stdout") : "data");
		buffer_free(&input);
		buffer_free(&decoded);
		buffer_free(&raw);
		return (1);
	}
	buffer_free(&input);
	buffer_free(&decoded);
	buffer_free(&raw);
	return (0);
}
