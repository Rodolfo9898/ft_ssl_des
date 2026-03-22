/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des.h                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 00:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/22 00:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DES_H
# define DES_H

# include "ft_ssl.h"

typedef enum e_des_mode
{
	DES_MODE_ECB,
	DES_MODE_CBC,
	DES_MODE_OFB
} t_des_mode;

typedef enum e_des_algo
{
	DES_ALGO_DES,
	DES_ALGO_3DES
} t_des_algo;

typedef struct s_des_options
{
	t_des_algo	algo;
	t_des_mode	mode;
	int			decrypt;
	int			base64;
	char		*input_path;
	char		*output_path;
	int			key_set;
	uint64_t	keys[3];
	int			pass_set;
	char		*password;
	int			salt_set;
	uint64_t	salt;
	int			iv_set;
	uint64_t	iv;
} t_des_options;

typedef struct s_des_subkeys
{
	uint64_t	k1[16];
	uint64_t	k2[16];
	uint64_t	k3[16];
} t_des_subkeys;

extern const int	g_des_ip[64];
extern const int	g_des_fp[64];
extern const int	g_des_e[48];
extern const int	g_des_p[32];
extern const int	g_des_pc1[56];
extern const int	g_des_pc2[48];
extern const int	g_des_shifts[16];
extern const int	g_des_sbox[8][64];

uint64_t			des_load_be64(const unsigned char *bytes);
void				des_store_be64(uint64_t value, unsigned char *bytes);
uint64_t			des_permute(uint64_t src, const int *table, int out_len,
						int in_len);
uint32_t			des_left_rotate28(uint32_t value, int shift);
void				des_compute_subkeys(uint64_t key, uint64_t subkeys[16]);
uint64_t			des_block_crypt(uint64_t block, const uint64_t subkeys[16],
						int decrypt);
uint64_t			des3_block_crypt(uint64_t block, const t_des_subkeys *keys,
						int decrypt);

int					des_parse_hex_bytes(const char *str, unsigned char *out,
						size_t out_len);
int					des_parse_hex64(const char *value, uint64_t *out);
int					des_read_random_salt(uint64_t *salt);
int					des_load_input(const char *path, t_buffer *buffer);
int					des_write_output(const char *path, const t_buffer *buffer);
int					des_is_salted_header(const t_buffer *input);
int					des_append_salted_header(const t_des_options *opts,
						t_buffer *buffer);
int					des_evp_bytes_to_material(const char *password, uint64_t salt,
						unsigned char *material, size_t need_len);
int					des_prepare_key_material(t_des_options *opts, t_buffer *input,
						int *derived_from_password);
void				des_init_defaults(t_des_options *opts,
						const t_command *command);
int					des_parse_options(t_des_options *opts, int argc, char **argv);
int					des_run_ecb_cbc(const unsigned char *data, size_t len,
						t_des_options *opts, t_buffer *out);
int					des_run_ofb(const unsigned char *data, size_t len,
						t_des_options *opts, t_buffer *out);
int					des_run_cipher_data(const unsigned char *data, size_t len,
						t_des_options *opts, t_buffer *out);
int					des_prepare_input_for_decrypt(t_des_options *opts,
						t_buffer *input, t_buffer *working);
int					des_wrap_encrypted_output(t_des_options *opts, t_buffer *raw,
						int derived_from_password, t_buffer *output);

#endif
