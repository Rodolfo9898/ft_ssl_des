/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kdf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/* The SHA-256 digest: each word of the state, most significant byte first */
static void	write_state_be(unsigned char *digest, const uint32_t *state)
{
	int	word;
	int	byte;

	word = 0;
	while (word < SHA256_STATE_WORDS)
	{
		byte = 0;
		while (byte < WORD_SIZE)
		{
			digest[word * WORD_SIZE + byte] = (state[word]
					>> (BYTE_BITS * (WORD_SIZE - 1 - byte))) % BYTE_RANGE;
			byte++;
		}
		word++;
	}
}

/* SHA-256 of data, with ft_ssl_md5's padding and block function */
static int	sha256_digest(const unsigned char *data, size_t len,
	unsigned char *digest)
{
	static const uint32_t	init[SHA256_STATE_WORDS] = {SHA256_H0, SHA256_H1,
		SHA256_H2, SHA256_H3, SHA256_H4, SHA256_H5, SHA256_H6, SHA256_H7};
	t_sha256_ctx			ctx;
	unsigned char			*padded;
	size_t					padded_len;
	size_t					offset;

	ft_memcpy(ctx.h, init, sizeof(init));
	padded = pad_message(data, len, &padded_len, ORDER_BIG_ENDIAN);
	if (padded == NULL)
		return (0);
	offset = 0;
	while (offset < padded_len)
	{
		sha256_process_block(&ctx, padded + offset);
		offset += BLOCK_SIZE;
	}
	free(padded);
	write_state_be(digest, ctx.h);
	return (1);
}

/*
** The input of a round: the digest before it (none at first), then the
** password, then the salt. Returns its length.
*/
static size_t	round_input(unsigned char *input, const unsigned char *digest,
	int first, const t_des_options *opts)
{
	size_t	len;

	len = 0;
	if (!first)
	{
		ft_memcpy(input, digest, SHA256_DIGEST_SIZE);
		len = SHA256_DIGEST_SIZE;
	}
	ft_memcpy(input + len, opts->password, ft_strlen(opts->password));
	len += ft_strlen(opts->password);
	ft_memcpy(input + len, opts->salt, SALT_SIZE);
	return (len + SALT_SIZE);
}

/*
** OpenSSL's PBKDF, EVP_BytesToKey with SHA-256 and one iteration (what
** openssl enc does without -pbkdf2, OpenSSL 3's default digest): D1 is
** SHA-256(password salt), each next D SHA-256(D before, password, salt), and
** the key then the IV are their bytes, need of them. 0 if malloc failed.
*/
int	des_evp_bytes_to_key(const t_des_options *opts, unsigned char *material,
	size_t need)
{
	unsigned char	digest[SHA256_DIGEST_SIZE];
	unsigned char	*input;
	size_t			done;
	size_t			chunk;

	input = malloc(SHA256_DIGEST_SIZE + ft_strlen(opts->password) + SALT_SIZE);
	if (input == NULL)
		return (0);
	done = 0;
	while (done < need)
	{
		if (!sha256_digest(input, round_input(input, digest, done == 0, opts),
				digest))
			break ;
		chunk = SHA256_DIGEST_SIZE;
		if (chunk > need - done)
			chunk = need - done;
		ft_memcpy(material + done, digest, chunk);
		done += chunk;
	}
	ft_bzero(digest, sizeof(digest));
	ft_bzero(input, SHA256_DIGEST_SIZE + ft_strlen(opts->password) + SALT_SIZE);
	free(input);
	return (done >= need);
}
