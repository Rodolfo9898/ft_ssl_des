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

#include "commands/des.h"

static void	init_buffer(t_buffer *buffer)
{
	buffer->data = NULL;
	buffer->len = 0;
	buffer->capacity = 0;
}

static int	finish_command(t_buffer *a, t_buffer *b, t_buffer *c, int code)
{
	buffer_free(a);
	buffer_free(b);
	buffer_free(c);
	return (code);
}

static int	run_encrypt_flow(t_des_options *opts, t_buffer *input,
	t_buffer *decoded, t_buffer *raw)
{
	int	derived;

	if (!des_prepare_key_material(opts, input, &derived))
		return (0);
	if (!des_run_cipher_data(input->data, input->len, opts, raw))
		return (0);
	if (!des_wrap_encrypted_output(opts, raw, derived, decoded))
		return (0);
	return (1);
}

static int	run_decrypt_flow(t_des_options *opts, t_buffer *input,
	t_buffer *decoded, t_buffer *raw)
{
	int	derived;

	if (!des_prepare_input_for_decrypt(opts, input, decoded))
		return (0);
	if (!des_prepare_key_material(opts, decoded, &derived))
		return (0);
	if (!des_run_cipher_data(decoded->data, decoded->len, opts, raw))
		return (0);
	return (1);
}

int	run_des_command(const t_command *command, int argc, char **argv)
{
	t_des_options	opts;
	t_buffer		input;
	t_buffer		decoded;
	t_buffer		raw;
	t_buffer		*result;

	des_init_defaults(&opts, command);
	init_buffer(&input);
	init_buffer(&decoded);
	init_buffer(&raw);
	if (!des_parse_options(&opts, argc, argv))
		return (ft_putendl_fd(2, "ft_ssl: des: invalid options"), 1);
	if (!des_load_input(opts.input_path, &input))
		return (print_system_error(command->name, opts.input_path), 1);
	if (opts.decrypt)
		result = &raw;
	else
		result = &decoded;
	if (opts.decrypt && !run_decrypt_flow(&opts, &input, &decoded, &raw))
		return (finish_command(&input, &decoded, &raw, 1));
	if (!opts.decrypt && !run_encrypt_flow(&opts, &input, &decoded, &raw))
		return (finish_command(&input, &decoded, &raw, 1));
	if (!des_write_output(opts.output_path, result))
		return (finish_command(&input, &decoded, &raw, 1));
	return (finish_command(&input, &decoded, &raw, 0));
}
