/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   options.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* The flags followed by a value: -i, -o, -k, -p, -s and -v */
static int	takes_value(const char *flag)
{
	return (flag_is(flag, FLAG_INPUT) || flag_is(flag, FLAG_OUTPUT)
		|| flag_is(flag, FLAG_KEY) || flag_is(flag, FLAG_PASSWORD)
		|| flag_is(flag, FLAG_SALT) || flag_is(flag, FLAG_IV));
}

/*
** -k, -s and -v: hexadecimal, like openssl -K, -S and -iv (p.10). The key
** has the size of the cipher's (8 bytes, 24 for Triple DES), the salt and
** the IV 8 bytes.
*/
static int	take_hex(t_des_options *opts, const char *flag, char *value)
{
	unsigned char	*field;
	size_t			size;

	field = opts->iv;
	size = DES_BLOCK_SIZE;
	if (flag_is(flag, FLAG_KEY))
	{
		field = opts->key;
		size = opts->cipher->key_size;
	}
	else if (flag_is(flag, FLAG_SALT))
		field = opts->salt;
	if (!hex_parse(value, field, size))
		return (cipher_arg_error(opts->cipher->name, MSG_BAD_HEX, value));
	if (flag_is(flag, FLAG_KEY))
		opts->key_set = 1;
	else if (flag_is(flag, FLAG_SALT))
		opts->salt_set = 1;
	else
		opts->iv_set = 1;
	return (1);
}

static int	take_value(t_des_options *opts, const char *flag, char *value)
{
	if (flag_is(flag, FLAG_INPUT))
		opts->input_path = value;
	else if (flag_is(flag, FLAG_OUTPUT))
		opts->output_path = value;
	else if (flag_is(flag, FLAG_PASSWORD))
		opts->password = value;
	else
		return (take_hex(opts, flag, value));
	return (1);
}

/*
** -a, -d, -e (the default), and the flags with a value, in any order, the
** last one winning. Anything else is an error, with the usage.
*/
int	des_parse_options(t_des_options *opts, int argc, char **argv)
{
	int	index;

	index = 0;
	while (index < argc)
	{
		if (flag_is(argv[index], FLAG_BASE64))
			opts->base64 = 1;
		else if (flag_is(argv[index], FLAG_DECRYPT))
			opts->direction = DIRECTION_DECRYPT;
		else if (flag_is(argv[index], FLAG_ENCRYPT))
			opts->direction = DIRECTION_ENCRYPT;
		else if (!takes_value(argv[index]))
			return (cipher_usage_error(opts->cipher->name, MSG_BAD_OPTION,
					argv[index], DES_USAGE));
		else if (index + 1 >= argc)
			return (cipher_usage_error(opts->cipher->name, MSG_NEEDS_ARG,
					argv[index], DES_USAGE));
		else if (!take_value(opts, argv[index], argv[index + 1]))
			return (0);
		else
			index++;
		index++;
	}
	return (1);
}
