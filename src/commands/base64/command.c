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

static int	load_input(char *input_path, t_buffer *buffer)
{
	if (input_path != NULL)
	{
		if (!read_file(input_path, buffer))
			return (print_system_error("base64", input_path), 0);
		return (1);
	}
	if (!read_stream(STDIN_FILENO, buffer))
		return (print_system_error("base64", "stdin"), 0);
	return (1);
}

static int	write_output(char *output_path, const t_buffer *buffer)
{
	int	fd;
	int	ok;

	if (output_path == NULL)
		return (write_all(STDOUT_FILENO, buffer->data, buffer->len));
	fd = open(output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
		return (print_system_error("base64", output_path), 0);
	ok = write_all(fd, buffer->data, buffer->len);
	if (!ok)
		print_system_error("base64", output_path);
	close(fd);
	return (ok);
}

static int	parse_options(int argc, char **argv, char **paths, int *decode)
{
	int	index;

	index = 0;
	while (index < argc)
	{
		if (ft_is_streq(argv[index], "-d"))
			*decode = 1;
		else if (ft_is_streq(argv[index], "-e"))
			*decode = 0;
		else if ((ft_is_streq(argv[index], "-i")
				|| ft_is_streq(argv[index], "-o")) && index + 1 < argc)
		{
			index++;
			if (argv[index - 1][1] == 'i')
				paths[0] = argv[index];
			else
				paths[1] = argv[index];
		}
		else
			return (0);
		index++;
	}
	return (1);
}

static int	process_data(int decode, t_buffer *input, t_buffer *output)
{
	if (decode == 1)
		return (base64_decode_data(input->data, input->len, output));
	if (!base64_encode_data(input->data, input->len, output))
		return (0);
	return (buffer_append(output, (unsigned char *)"\n", 1));
}

int	run_base64_command(const t_command *command, int argc, char **argv)
{
	t_buffer	input;
	t_buffer	output;
	char		*paths[2];
	int			decode;

	(void)command;
	input.data = NULL;
	input.len = 0;
	input.capacity = 0;
	output.data = NULL;
	output.len = 0;
	output.capacity = 0;
	paths[0] = NULL;
	paths[1] = NULL;
	decode = 0;
	if (!parse_options(argc, argv, paths, &decode)
		|| !load_input(paths[0], &input)
		|| !process_data(decode, &input, &output)
		|| !write_output(paths[1], &output))
		return (print_system_error("base64", "data"),
			buffer_free(&input), buffer_free(&output), 1);
	buffer_free(&input);
	buffer_free(&output);
	return (0);
}
