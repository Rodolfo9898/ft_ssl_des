/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_ssl_des.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_SSL_DES_H
# define FT_SSL_DES_H

# include "core/des_structs.h"

/*
** Only what more than one file uses: everything else is static in its file.
** md5, sha256, the buffers, the output and the command table's
** execute_command_argv() are declared by ft_ssl_md5 (libs/ft_ssl_md5), the
** string and memory helpers by libft (libs/libft).
*/

/* From utils/errors.c: each prints "ft_ssl: command: ..." and returns 0 */
int					cipher_error(char *command, char *message);
int					cipher_arg_error(char *command, char *message, char *arg);
int					cipher_usage_error(char *command, char *message, char *arg,
						char *usage);
int					cipher_system_error(char *command, char *target);
int					cipher_invalid_data(void);

/* From utils/args.c */
int					flag_is(const char *arg, const char *flag);
int					is_space(unsigned char c);
int					hex_parse(const char *str, unsigned char *out,
						size_t out_len);

/* From utils/cipher_io.c */
int					cipher_read_input(char *command, char *path,
						t_buffer *in);
int					cipher_write_output(char *command, char *path,
						const t_buffer *out);

/* From base64/encode.c and base64/decode.c */
int					base64_encode(const unsigned char *data, size_t len,
						t_buffer *out);
int					base64_decode(const unsigned char *data, size_t len,
						t_buffer *out);

/* From base64/command.c */
int					run_base64(int argc, char **argv);

/* From des/command.c: the commands, and des/run.c, what they all run */
int					run_des(int argc, char **argv);
int					run_des_ecb(int argc, char **argv);
int					run_des_cbc(int argc, char **argv);
int					run_cipher(const t_cipher *cipher, int argc, char **argv);

/* From des/options.c */
int					des_parse_options(t_des_options *opts, int argc,
						char **argv);

/* From des/key.c, des/password.c, des/salt.c and des/kdf.c */
int					des_prepare_key(t_des_options *opts, const t_buffer *data,
						size_t *skip);
int					des_get_password(t_des_options *opts);
int					des_random_salt(t_des_options *opts);
int					des_append_header(const t_des_options *opts,
						t_buffer *out);
int					des_read_header(t_des_options *opts, const t_buffer *data,
						size_t *skip);
int					des_evp_bytes_to_key(const t_des_options *opts,
						unsigned char *material, size_t need);

/* From des/tables.c and des/bits.c */
const t_des_tables	*des_tables(void);
uint64_t			des_load_be64(const unsigned char *bytes);
void				des_store_be64(uint64_t value, unsigned char *bytes);
uint64_t			des_permute(uint64_t src, const int *table, int out_bits,
						int in_bits);
uint32_t			des_rotate28(uint32_t value, int shift);

/* From des/block.c, des/subkeys.c and des/modes.c */
uint64_t			des_crypt_single(uint64_t block, const uint64_t *subkeys,
						t_direction direction);
uint64_t			des_crypt_block(uint64_t block, const t_des_key *key,
						t_direction direction);
void				des_init_job(t_des_job *job, const t_des_options *opts);
int					des_run_blocks(const t_des_job *job, const t_buffer *in,
						t_buffer *out);

/* Hidden from the mandatory build: only with -D BONUS_MODE=1 */
# if BONUS_MODE

/* From bonus/ofb_bonus.c */
int					des_run_ofb(const t_des_job *job, const t_buffer *in,
						t_buffer *out);
int					run_des_ofb(int argc, char **argv);

/* From bonus/des3_bonus.c */
uint64_t			des3_crypt_block(uint64_t block, const t_des_key *key,
						t_direction direction);
int					run_des3(int argc, char **argv);
int					run_des3_ecb(int argc, char **argv);
int					run_des3_cbc(int argc, char **argv);
int					run_des3_ofb(int argc, char **argv);

# endif

#endif
