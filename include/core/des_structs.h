/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_structs.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DES_STRUCTS_H
# define DES_STRUCTS_H

# include "des_defines.h"
# include "des_libs.h"
# include "des_enums.h"

/* The flags of base64: decode or encode, and the files given */
typedef struct s_base64_options
{
	int		decode;
	char	*input_path;
	char	*output_path;
}	t_base64_options;

/* The round keys of each of the keys: one for DES, three for Triple DES */
typedef struct s_des_key
{
	uint64_t	subkeys[DES_MAX_KEYS][DES_ROUNDS];
}	t_des_key;

/* What a mode needs to run: the cipher, its round keys, the IV, the way */
typedef struct s_des_job
{
	const struct s_cipher	*cipher;
	t_des_key				key;
	uint64_t				iv;
	t_direction				direction;
}	t_des_job;

/*
** One cipher command: its name, its mode, the size of its key, the function
** that encrypts or decrypts one block (DES or Triple DES), and the one that
** runs the mode over the data (ECB and CBC, or OFB).
*/
typedef struct s_cipher
{
	char		*name;
	t_des_mode	mode;
	size_t		key_size;
	uint64_t	(*crypt_block)(uint64_t block, const t_des_key *key,
			t_direction direction);
	int			(*run_mode)(const t_des_job *job, const t_buffer *in,
			t_buffer *out);
}	t_cipher;

/*
** The flags of a cipher command, then what they set: the key, the salt and
** the IV in bytes, each with whether it was given (or derived). The password
** is the one of -p, or a copy of the one typed: then it must be freed.
*/
typedef struct s_des_options
{
	const t_cipher	*cipher;
	t_direction		direction;
	int				base64;
	char			*input_path;
	char			*output_path;
	char			*password;
	int				password_owned;
	int				key_set;
	int				salt_set;
	int				iv_set;
	int				salted_header;
	unsigned char	key[DES_MAX_KEY_SIZE];
	unsigned char	salt[SALT_SIZE];
	unsigned char	iv[DES_BLOCK_SIZE];
}	t_des_options;

/* The tables of FIPS 46-3, filled from des_defines.h in des/tables.c */
typedef struct s_des_tables
{
	int	ip[DES_BLOCK_BITS];
	int	fp[DES_BLOCK_BITS];
	int	e[DES_SUBKEY_BITS];
	int	p[DES_HALF_BITS];
	int	pc1[DES_PC1_BITS];
	int	pc2[DES_SUBKEY_BITS];
	int	shifts[DES_ROUNDS];
	int	sbox[DES_SBOXES][DES_SBOX_SIZE];
}	t_des_tables;

#endif
