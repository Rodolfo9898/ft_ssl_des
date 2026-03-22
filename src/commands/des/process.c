/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

int	des_prepare_input_for_decrypt(t_des_options *opts, t_buffer *input,
	t_buffer *working)
{
	if (!opts->base64)
		return (buffer_append(working, input->data, input->len));
	return (base64_decode_data(input->data, input->len, working));
}

int	des_run_cipher_data(const unsigned char *data, size_t len,
	t_des_options *opts, t_buffer *out)
{
	if (opts->mode == DES_MODE_OFB)
		return (des_run_ofb(data, len, opts, out));
	return (des_run_ecb_cbc(data, len, opts, out));
}

int	des_wrap_encrypted_output(t_des_options *opts, t_buffer *raw,
	int derived_from_password, t_buffer *output)
{
	t_buffer	wrapped;

	wrapped.data = NULL;
	wrapped.len = 0;
	wrapped.capacity = 0;
	if (derived_from_password)
		if (!des_append_salted_header(opts, &wrapped))
			return (0);
	if (!buffer_append(&wrapped, raw->data, raw->len))
		return (buffer_free(&wrapped), 0);
	if (!opts->base64)
	{
		if (!buffer_append(output, wrapped.data, wrapped.len))
			return (buffer_free(&wrapped), 0);
	}
	else if (!base64_encode_data(wrapped.data, wrapped.len, output)
		|| !buffer_append(output, (const unsigned char *)"\n", 1))
		return (buffer_free(&wrapped), 0);
	buffer_free(&wrapped);
	return (1);
}
