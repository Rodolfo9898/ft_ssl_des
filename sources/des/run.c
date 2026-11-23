/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   run.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** Encrypting: the key (and the salt when it is random, written first as
** OpenSSL does), the mode, then base64 with -a.
*/
static int	encrypt_data(t_des_options *opts, const t_buffer *in,
	t_buffer *out)
{
	t_des_job	job;
	t_buffer	raw;
	int			ok;

	if (!des_prepare_key(opts, NULL, NULL))
		return (0);
	des_init_job(&job, opts);
	ft_bzero(&raw, sizeof(raw));
	ok = (!opts->salted_header || des_append_header(opts, &raw))
		&& opts->cipher->run_mode(&job, in, &raw);
	if (ok && opts->base64)
		ok = base64_encode(raw.data, raw.len, out);
	else if (ok)
		ok = buffer_append(out, raw.data, raw.len);
	ft_bzero(&job, sizeof(job));
	buffer_free(&raw);
	if (!ok)
		return (cipher_system_error(opts->cipher->name, NULL));
	return (1);
}

/* The ciphertext: decoded from base64 with -a, as it is without */
static int	load_ciphertext(const t_des_options *opts, const t_buffer *in,
	t_buffer *data)
{
	if (!opts->base64 && buffer_append(data, in->data, in->len))
		return (1);
	if (opts->base64 && base64_decode(in->data, in->len, data))
		return (1);
	if (errno == EINVAL)
		return (cipher_error(opts->cipher->name, MSG_BAD_BASE64));
	return (cipher_system_error(opts->cipher->name, NULL));
}

/*
** Decrypting: the ciphertext, the key (from its "Salted__" header with a
** password), then the mode on what follows the header. A wrong key or a
** damaged ciphertext gives a wrong padding: "bad decrypt", like OpenSSL.
*/
static int	decrypt_data(t_des_options *opts, const t_buffer *in,
	t_buffer *out)
{
	t_des_job	job;
	t_buffer	data;
	t_buffer	body;
	size_t		skip;
	int			ok;

	ft_bzero(&data, sizeof(data));
	ok = load_ciphertext(opts, in, &data)
		&& des_prepare_key(opts, &data, &skip);
	if (ok)
	{
		des_init_job(&job, opts);
		ft_bzero(&body, sizeof(body));
		body.data = data.data + skip;
		body.len = data.len - skip;
		ok = opts->cipher->run_mode(&job, &body, out);
		ft_bzero(&job, sizeof(job));
		if (!ok && errno == EINVAL)
			cipher_error(opts->cipher->name, MSG_BAD_DECRYPT);
		else if (!ok)
			cipher_system_error(opts->cipher->name, NULL);
	}
	buffer_free(&data);
	return (ok);
}

/* Frees what the command allocated, the password wiped first */
static int	finish(t_des_options *opts, t_buffer *in, t_buffer *out, int ok)
{
	if (opts->password_owned)
	{
		ft_bzero(opts->password, ft_strlen(opts->password));
		free(opts->password);
	}
	ft_bzero(opts->key, sizeof(opts->key));
	buffer_free(in);
	buffer_free(out);
	if (!ok)
		return (EXIT_FAILURE);
	return (EXIT_SUCCESS);
}

/*
** Every cipher command: its flags, its input (-i or the standard input),
** encrypted or decrypted, then its output (-o or the standard output).
*/
int	run_cipher(const t_cipher *cipher, int argc, char **argv)
{
	t_des_options	opts;
	t_buffer		in;
	t_buffer		out;
	int				ok;

	ft_bzero(&opts, sizeof(opts));
	ft_bzero(&in, sizeof(in));
	ft_bzero(&out, sizeof(out));
	opts.cipher = cipher;
	if (!des_parse_options(&opts, argc, argv))
		return (EXIT_FAILURE);
	ok = cipher_read_input(cipher->name, opts.input_path, &in);
	if (ok && opts.direction == DIRECTION_DECRYPT)
		ok = decrypt_data(&opts, &in, &out);
	else if (ok)
		ok = encrypt_data(&opts, &in, &out);
	ok = ok && cipher_write_output(cipher->name, opts.output_path, &out);
	return (finish(&opts, &in, &out, ok));
}
