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

typedef struct s_base64_options
{
	int		decode;
	char	*input_path;
	char	*output_path;
}t_base64_options;

static int	load_input(const t_base64_options *opts, t_buffer *buffer)
{
	if (opts->input_path != NULL)
	{
		if (!read_file(opts->input_path, buffer))
		{
			print_system_error("base64", opts->input_path);
			return (0);
		}
		return (1);
	}
	if (!read_stream(STDIN_FILENO, buffer))
	{
		print_system_error("base64", "stdin");
		return (0);
	}
	return (1);
}

static int	write_output(const t_base64_options *opts, const t_buffer *buffer)
{
	int	fd;
	int	ok;

	if (opts->output_path == NULL)
		return (write_all(STDOUT_FILENO, buffer->data, buffer->len));
	fd = open(opts->output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (print_system_error("base64", opts->output_path), 0);
	ok = write_all(fd, buffer->data, buffer->len);
	if (!ok)
		print_system_error("base64", opts->output_path);
	close(fd);
	return (ok);
}

static int	parse_options(int argc, char **argv, t_base64_options *opts)
{
	int	index;

	index = 0;
	while (index < argc)
	{
		if (ft_is_streq(argv[index], "-d"))
			opts->decode = 1;
		else if (ft_is_streq(argv[index], "-e"))
			opts->decode = 0;
		else if ((ft_is_streq(argv[index], "-i") || ft_is_streq(argv[index], "-o"))
			&& index + 1 < argc)
		{
			if (argv[index][1] == 'i')
				opts->input_path = argv[index + 1];
			else
				opts->output_path = argv[index + 1];
			index++;
		}
		else
		{
			ft_putstr_fd(2, "ft_ssl: base64: ");
			ft_putstr_fd(2, argv[index]);
			ft_putendl_fd(2, ": invalid option or missing argument");
			return (0);
		}
		index++;
	}
	return (1);
}

int	run_base64_command(const t_command *command, int argc, char **argv)
{
	t_base64_options	opts;
	t_buffer			input;
	t_buffer			output;
	int				ok;

	(void)command;
	opts.decode = 0;
	opts.input_path = NULL;
	opts.output_path = NULL;
	input.data = NULL;
	input.len = 0;
	input.capacity = 0;
	output.data = NULL;
	output.len = 0;
	output.capacity = 0;
	if (!parse_options(argc, argv, &opts) || !load_input(&opts, &input))
		return (buffer_free(&input), 1);
	if (opts.decode)
		ok = base64_decode_data(input.data, input.len, &output);
	else
	{
		ok = base64_encode_data(input.data, input.len, &output);
		if (ok)
			ok = buffer_append(&output, (const unsigned char *)"\n", 1);
	}
	if (!ok)
		return (print_system_error("base64", "data"),
			buffer_free(&input), buffer_free(&output), 1);
	ok = write_output(&opts, &output);
	buffer_free(&input);
	buffer_free(&output);
	if (!ok)
		return (1);
	return (0);
}
