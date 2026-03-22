/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/des.h"

static int	parse_k_value(t_des_options *opts, const char *value)
{
	unsigned char	bytes[24];

	if (opts->algo == DES_ALGO_3DES)
	{
		if (!des_parse_hex_bytes(value, bytes, 24))
			return (0);
		opts->keys[0] = des_load_be64(bytes);
		opts->keys[1] = des_load_be64(bytes + 8);
		opts->keys[2] = des_load_be64(bytes + 16);
	}
	else
	{
		if (!des_parse_hex_bytes(value, bytes, 8))
			return (0);
		opts->keys[0] = des_load_be64(bytes);
		opts->keys[1] = 0;
		opts->keys[2] = 0;
	}
	opts->key_set = 1;
	return (1);
}

void	des_init_defaults(t_des_options *opts, const t_command *command)
{
	ft_memset(opts, 0, sizeof(*opts));
	opts->mode = DES_MODE_CBC;
	opts->algo = DES_ALGO_DES;
	if (ft_is_streq(command->name, "des-ecb")
		|| ft_is_streq(command->name, "des3-ecb"))
		opts->mode = DES_MODE_ECB;
	if (ft_is_streq(command->name, "des-ofb")
		|| ft_is_streq(command->name, "des3-ofb"))
		opts->mode = DES_MODE_OFB;
	if (ft_is_streq(command->name, "des3")
		|| ft_is_streq(command->name, "des3-cbc")
		|| ft_is_streq(command->name, "des3-ecb")
		|| ft_is_streq(command->name, "des3-ofb"))
		opts->algo = DES_ALGO_3DES;
}

static int	parse_flag_with_arg(t_des_options *opts, int argc,
	char **argv, int *index)
{
	const char	*flag;
	const char	*val;

	flag = argv[*index];
	if (*index + 1 >= argc)
		return (0);
	val = argv[++(*index)];
	if (ft_is_streq(flag, "-i"))
		opts->input_path = (char *)val;
	else if (ft_is_streq(flag, "-o"))
		opts->output_path = (char *)val;
	else if (ft_is_streq(flag, "-k"))
		return (parse_k_value(opts, val));
	else if (ft_is_streq(flag, "-p"))
		opts->password = (opts->pass_set = 1, (char *)val);
	else if (ft_is_streq(flag, "-s"))
		return (opts->salt_set = des_parse_hex64(val, &opts->salt));
	else if (ft_is_streq(flag, "-v"))
		return (opts->iv_set = des_parse_hex64(val, &opts->iv));
	else
		return (0);
	return (1);
}

int	des_parse_options(t_des_options *opts, int argc, char **argv)
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
		else if (!parse_flag_with_arg(opts, argc, argv, &index))
			return (0);
		index++;
	}
	return (1);
}
