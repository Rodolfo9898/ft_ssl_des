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

/* The file of -i or -o */
static void	take_path(t_base64_options *opts, const char *flag, char *path)
{
	if (flag_is(flag, FLAG_INPUT))
		opts->input_path = path;
	else
		opts->output_path = path;
}

/*
** -d, -e (the default), -i file and -o file (ft_ssl_des.pdf p.9), in any
** order, the last one winning. Anything else is an error, with the usage.
*/
static int	parse_base64_options(t_base64_options *opts, int argc,
	char **argv)
{
	int	index;

	index = 0;
	while (index < argc)
	{
		if (flag_is(argv[index], FLAG_DECODE))
			opts->decode = 1;
		else if (flag_is(argv[index], FLAG_ENCODE))
			opts->decode = 0;
		else if (!flag_is(argv[index], FLAG_INPUT)
			&& !flag_is(argv[index], FLAG_OUTPUT))
			return (cipher_usage_error(BASE64_NAME, MSG_BAD_OPTION,
					argv[index], BASE64_USAGE));
		else if (index + 1 >= argc)
			return (cipher_usage_error(BASE64_NAME, MSG_NEEDS_ARG,
					argv[index], BASE64_USAGE));
		else
		{
			take_path(opts, argv[index], argv[index + 1]);
			index++;
		}
		index++;
	}
	return (1);
}

/* Encodes or decodes in into out: says why when it cannot */
static int	base64_transform(const t_base64_options *opts, const t_buffer *in,
	t_buffer *out)
{
	if (!opts->decode && base64_encode(in->data, in->len, out))
		return (1);
	if (opts->decode && base64_decode(in->data, in->len, out))
		return (1);
	if (errno == EINVAL)
		return (cipher_error(BASE64_NAME, MSG_BAD_BASE64));
	return (cipher_system_error(BASE64_NAME, NULL));
}

/* The base64 command: reads everything, transforms it, writes it */
int	run_base64(int argc, char **argv)
{
	t_base64_options	opts;
	t_buffer			in;
	t_buffer			out;
	int					ok;

	ft_bzero(&opts, sizeof(opts));
	ft_bzero(&in, sizeof(in));
	ft_bzero(&out, sizeof(out));
	if (!parse_base64_options(&opts, argc, argv))
		return (EXIT_FAILURE);
	ok = cipher_read_input(BASE64_NAME, opts.input_path, &in)
		&& base64_transform(&opts, &in, &out)
		&& cipher_write_output(BASE64_NAME, opts.output_path, &out);
	buffer_free(&in);
	buffer_free(&out);
	if (!ok)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}
