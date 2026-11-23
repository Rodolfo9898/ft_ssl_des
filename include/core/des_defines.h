/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_defines.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DES_DEFINES_H
# define DES_DEFINES_H

/*
** The constants of ft_ssl_des. The ones it shares with ft_ssl_md5 (BONUS_MODE,
** MIN_ARGC, MSG_USAGE, BYTE_BITS, BYTE_RANGE, HEX_BASE, HEX_PER_BYTE, the
** SHA-256 constants...) come from libs/ft_ssl_md5/include/core/defines.h.
*/

/* The cipher commands, by the name they print and prompt with */
# define BASE64_NAME "base64"
# define DES_NAME "des"
# define DES_ECB_NAME "des-ecb"
# define DES_CBC_NAME "des-cbc"

/* The list of commands after an invalid one (ft_ssl_des.pdf p.7) */
# define MSG_INVALID_START "ft_ssl: Error: '"
# define MSG_INVALID_END "' is an invalid command."
# define MSG_STANDARD "Standard commands:"
# define MSG_DIGEST "Message Digest commands:"
# define MSG_CIPHER "Cipher commands:"

/* The flags of base64 (p.9) and of des (p.10) */
# define FLAG_DECODE "-d"
# define FLAG_ENCODE "-e"
# define FLAG_BASE64 "-a"
# define FLAG_DECRYPT "-d"
# define FLAG_ENCRYPT "-e"
# define FLAG_INPUT "-i"
# define FLAG_OUTPUT "-o"
# define FLAG_KEY "-k"
# define FLAG_PASSWORD "-p"
# define FLAG_SALT "-s"
# define FLAG_IV "-v"

/* What follows "usage: ft_ssl command " after a bad option */
# define MSG_USAGE_START "usage: ft_ssl "
# define BASE64_USAGE "[-d | -e] [-i file] [-o file]"
# define DES_USAGE "[-a] [-d | -e] [-i in] [-o out] [-k|-p|-s|-v value]"

/* The errors: "ft_ssl: command: message", as OpenSSL words them */
# define MSG_BAD_OPTION "invalid option"
# define MSG_NEEDS_ARG "option requires an argument"
# define MSG_BAD_HEX "invalid hex value"
# define MSG_BAD_BASE64 "invalid base64 input"
# define MSG_BAD_DECRYPT "bad decrypt"
# define MSG_BAD_MAGIC "bad magic number"
# define MSG_IV_UNDEFINED "iv undefined"
# define MSG_VERIFY_FAILURE "Verify failure"
# define MSG_STDIN "stdin"
# define MSG_STDOUT "stdout"
# define MSG_PASSWORD "password"

/*
** The password prompts, as OpenSSL shows them: "enter des-ecb encryption
** password:", then "Verifying - enter des-ecb encryption password:".
*/
# define PROMPT_SIZE 128
# define PROMPT_ENTER "enter "
# define PROMPT_VERIFY "Verifying - "
# define PROMPT_ENCRYPT " encryption password:"
# define PROMPT_DECRYPT " decryption password:"

/* The files: -o creates with these permissions, the salt comes from urandom */
# define OUTPUT_FILE_MODE 0644
# define RANDOM_DEVICE "/dev/urandom"

/*
** Base64 (RFC 4648): three bytes become four characters of six bits each, '='
** pads the last group, and the lines are 64 characters long, like OpenSSL's.
** The 64 characters: A-Z are 0 to 25, a-z 26 to 51, 0-9 52 to 61, '+' 62,
** '/' 63. Spaces, tabs and newlines in the input are skipped.
*/
# define BASE64_GROUP_SIZE 3
# define BASE64_QUAD_SIZE 4
# define BASE64_SEXTET_BITS 6
# define BASE64_SEXTET_MASK 0x3F
# define BASE64_MAX_PADS 2
# define BASE64_PAD '='
# define BASE64_LINE_WIDTH 64
# define BASE64_LETTERS 26
# define BASE64_DIGITS_START 52
# define BASE64_DIGITS 10
# define BASE64_PLUS_VALUE 62
# define BASE64_SLASH_VALUE 63
# define BASE64_PLUS '+'
# define BASE64_SLASH '/'
# define WHITESPACE " \t\n\v\f\r"

/*
** DES (FIPS 46-3): 64-bit blocks, 16 rounds, a 64-bit key of which PC1 keeps
** 56 bits, split in two halves of 28 that give a 48-bit key for each round.
** The 32-bit half block is expanded to 48 bits, then eight S-boxes turn
** 6 bits each back into 4: the row is the first and the last of the 6 bits,
** the column the 4 in between.
*/
# define DES_BLOCK_SIZE 8
# define DES_BLOCK_BITS 64
# define DES_HALF_BITS 32
# define DES_HALF_MASK 0xFFFFFFFF
# define DES_KEY_SIZE 8
# define DES_MAX_KEY_SIZE 24
# define DES_MAX_KEYS 3
# define DES_ROUNDS 16
# define DES_PC1_BITS 56
# define DES_HALF_KEY_BITS 28
# define DES_HALF_KEY_MASK 0x0FFFFFFF
# define DES_SUBKEY_BITS 48
# define DES_SBOXES 8
# define DES_SBOX_SIZE 64
# define DES_SBOX_INPUT_BITS 6
# define DES_SBOX_INPUT_MASK 0x3F
# define DES_SBOX_OUTPUT_BITS 4
# define DES_SBOX_ROW_HIGH 0x20
# define DES_SBOX_ROW_SHIFT 4
# define DES_SBOX_ROW_LOW 0x01
# define DES_SBOX_COLUMNS 16
# define DES_SBOX_COLUMN_MASK 0x0F

/*
** The "Salted__" header OpenSSL writes before data encrypted with a password,
** followed by the 8 bytes of the salt (man enc).
*/
# define SALT_MAGIC "Salted__"
# define SALT_MAGIC_SIZE 8
# define SALT_SIZE 8
# define SALT_HEADER_SIZE 16

/*
** Only with -D BONUS_MODE=1: the other modes and Triple DES (E-D-E with three
** keys, 24 bytes), named as OpenSSL names them.
*/
# if BONUS_MODE
#  define DES_OFB_NAME "des-ofb"
#  define DES3_NAME "des3"
#  define DES3_ECB_NAME "des3-ecb"
#  define DES3_CBC_NAME "des3-cbc"
#  define DES3_OFB_NAME "des3-ofb"
#  define DES3_KEY_SIZE 24
# endif

/*
** The tables of FIPS 46-3, entry by entry: the bit positions count from 1,
** the most significant bit of the input first.
*/

/* The initial permutation IP: bit i of the output is bit IP_i of the block */
# define DES_IP_0 58
# define DES_IP_1 50
# define DES_IP_2 42
# define DES_IP_3 34
# define DES_IP_4 26
# define DES_IP_5 18
# define DES_IP_6 10
# define DES_IP_7 2
# define DES_IP_8 60
# define DES_IP_9 52
# define DES_IP_10 44
# define DES_IP_11 36
# define DES_IP_12 28
# define DES_IP_13 20
# define DES_IP_14 12
# define DES_IP_15 4
# define DES_IP_16 62
# define DES_IP_17 54
# define DES_IP_18 46
# define DES_IP_19 38
# define DES_IP_20 30
# define DES_IP_21 22
# define DES_IP_22 14
# define DES_IP_23 6
# define DES_IP_24 64
# define DES_IP_25 56
# define DES_IP_26 48
# define DES_IP_27 40
# define DES_IP_28 32
# define DES_IP_29 24
# define DES_IP_30 16
# define DES_IP_31 8
# define DES_IP_32 57
# define DES_IP_33 49
# define DES_IP_34 41
# define DES_IP_35 33
# define DES_IP_36 25
# define DES_IP_37 17
# define DES_IP_38 9
# define DES_IP_39 1
# define DES_IP_40 59
# define DES_IP_41 51
# define DES_IP_42 43
# define DES_IP_43 35
# define DES_IP_44 27
# define DES_IP_45 19
# define DES_IP_46 11
# define DES_IP_47 3
# define DES_IP_48 61
# define DES_IP_49 53
# define DES_IP_50 45
# define DES_IP_51 37
# define DES_IP_52 29
# define DES_IP_53 21
# define DES_IP_54 13
# define DES_IP_55 5
# define DES_IP_56 63
# define DES_IP_57 55
# define DES_IP_58 47
# define DES_IP_59 39
# define DES_IP_60 31
# define DES_IP_61 23
# define DES_IP_62 15
# define DES_IP_63 7

/* The final permutation, IP inverted */
# define DES_FP_0 40
# define DES_FP_1 8
# define DES_FP_2 48
# define DES_FP_3 16
# define DES_FP_4 56
# define DES_FP_5 24
# define DES_FP_6 64
# define DES_FP_7 32
# define DES_FP_8 39
# define DES_FP_9 7
# define DES_FP_10 47
# define DES_FP_11 15
# define DES_FP_12 55
# define DES_FP_13 23
# define DES_FP_14 63
# define DES_FP_15 31
# define DES_FP_16 38
# define DES_FP_17 6
# define DES_FP_18 46
# define DES_FP_19 14
# define DES_FP_20 54
# define DES_FP_21 22
# define DES_FP_22 62
# define DES_FP_23 30
# define DES_FP_24 37
# define DES_FP_25 5
# define DES_FP_26 45
# define DES_FP_27 13
# define DES_FP_28 53
# define DES_FP_29 21
# define DES_FP_30 61
# define DES_FP_31 29
# define DES_FP_32 36
# define DES_FP_33 4
# define DES_FP_34 44
# define DES_FP_35 12
# define DES_FP_36 52
# define DES_FP_37 20
# define DES_FP_38 60
# define DES_FP_39 28
# define DES_FP_40 35
# define DES_FP_41 3
# define DES_FP_42 43
# define DES_FP_43 11
# define DES_FP_44 51
# define DES_FP_45 19
# define DES_FP_46 59
# define DES_FP_47 27
# define DES_FP_48 34
# define DES_FP_49 2
# define DES_FP_50 42
# define DES_FP_51 10
# define DES_FP_52 50
# define DES_FP_53 18
# define DES_FP_54 58
# define DES_FP_55 26
# define DES_FP_56 33
# define DES_FP_57 1
# define DES_FP_58 41
# define DES_FP_59 9
# define DES_FP_60 49
# define DES_FP_61 17
# define DES_FP_62 57
# define DES_FP_63 25

/* The expansion E: the 32 bits of a half block spread over 48 */
# define DES_E_0 32
# define DES_E_1 1
# define DES_E_2 2
# define DES_E_3 3
# define DES_E_4 4
# define DES_E_5 5
# define DES_E_6 4
# define DES_E_7 5
# define DES_E_8 6
# define DES_E_9 7
# define DES_E_10 8
# define DES_E_11 9
# define DES_E_12 8
# define DES_E_13 9
# define DES_E_14 10
# define DES_E_15 11
# define DES_E_16 12
# define DES_E_17 13
# define DES_E_18 12
# define DES_E_19 13
# define DES_E_20 14
# define DES_E_21 15
# define DES_E_22 16
# define DES_E_23 17
# define DES_E_24 16
# define DES_E_25 17
# define DES_E_26 18
# define DES_E_27 19
# define DES_E_28 20
# define DES_E_29 21
# define DES_E_30 20
# define DES_E_31 21
# define DES_E_32 22
# define DES_E_33 23
# define DES_E_34 24
# define DES_E_35 25
# define DES_E_36 24
# define DES_E_37 25
# define DES_E_38 26
# define DES_E_39 27
# define DES_E_40 28
# define DES_E_41 29
# define DES_E_42 28
# define DES_E_43 29
# define DES_E_44 30
# define DES_E_45 31
# define DES_E_46 32
# define DES_E_47 1

/* The permutation P of the 32 bits out of the S-boxes */
# define DES_P_0 16
# define DES_P_1 7
# define DES_P_2 20
# define DES_P_3 21
# define DES_P_4 29
# define DES_P_5 12
# define DES_P_6 28
# define DES_P_7 17
# define DES_P_8 1
# define DES_P_9 15
# define DES_P_10 23
# define DES_P_11 26
# define DES_P_12 5
# define DES_P_13 18
# define DES_P_14 31
# define DES_P_15 10
# define DES_P_16 2
# define DES_P_17 8
# define DES_P_18 24
# define DES_P_19 14
# define DES_P_20 32
# define DES_P_21 27
# define DES_P_22 3
# define DES_P_23 9
# define DES_P_24 19
# define DES_P_25 13
# define DES_P_26 30
# define DES_P_27 6
# define DES_P_28 22
# define DES_P_29 11
# define DES_P_30 4
# define DES_P_31 25

/* Permuted choice 1: the 56 key bits kept, parity bits dropped */
# define DES_PC1_0 57
# define DES_PC1_1 49
# define DES_PC1_2 41
# define DES_PC1_3 33
# define DES_PC1_4 25
# define DES_PC1_5 17
# define DES_PC1_6 9
# define DES_PC1_7 1
# define DES_PC1_8 58
# define DES_PC1_9 50
# define DES_PC1_10 42
# define DES_PC1_11 34
# define DES_PC1_12 26
# define DES_PC1_13 18
# define DES_PC1_14 10
# define DES_PC1_15 2
# define DES_PC1_16 59
# define DES_PC1_17 51
# define DES_PC1_18 43
# define DES_PC1_19 35
# define DES_PC1_20 27
# define DES_PC1_21 19
# define DES_PC1_22 11
# define DES_PC1_23 3
# define DES_PC1_24 60
# define DES_PC1_25 52
# define DES_PC1_26 44
# define DES_PC1_27 36
# define DES_PC1_28 63
# define DES_PC1_29 55
# define DES_PC1_30 47
# define DES_PC1_31 39
# define DES_PC1_32 31
# define DES_PC1_33 23
# define DES_PC1_34 15
# define DES_PC1_35 7
# define DES_PC1_36 62
# define DES_PC1_37 54
# define DES_PC1_38 46
# define DES_PC1_39 38
# define DES_PC1_40 30
# define DES_PC1_41 22
# define DES_PC1_42 14
# define DES_PC1_43 6
# define DES_PC1_44 61
# define DES_PC1_45 53
# define DES_PC1_46 45
# define DES_PC1_47 37
# define DES_PC1_48 29
# define DES_PC1_49 21
# define DES_PC1_50 13
# define DES_PC1_51 5
# define DES_PC1_52 28
# define DES_PC1_53 20
# define DES_PC1_54 12
# define DES_PC1_55 4

/* Permuted choice 2: the 48 bits of each round key */
# define DES_PC2_0 14
# define DES_PC2_1 17
# define DES_PC2_2 11
# define DES_PC2_3 24
# define DES_PC2_4 1
# define DES_PC2_5 5
# define DES_PC2_6 3
# define DES_PC2_7 28
# define DES_PC2_8 15
# define DES_PC2_9 6
# define DES_PC2_10 21
# define DES_PC2_11 10
# define DES_PC2_12 23
# define DES_PC2_13 19
# define DES_PC2_14 12
# define DES_PC2_15 4
# define DES_PC2_16 26
# define DES_PC2_17 8
# define DES_PC2_18 16
# define DES_PC2_19 7
# define DES_PC2_20 27
# define DES_PC2_21 20
# define DES_PC2_22 13
# define DES_PC2_23 2
# define DES_PC2_24 41
# define DES_PC2_25 52
# define DES_PC2_26 31
# define DES_PC2_27 37
# define DES_PC2_28 47
# define DES_PC2_29 55
# define DES_PC2_30 30
# define DES_PC2_31 40
# define DES_PC2_32 51
# define DES_PC2_33 45
# define DES_PC2_34 33
# define DES_PC2_35 48
# define DES_PC2_36 44
# define DES_PC2_37 49
# define DES_PC2_38 39
# define DES_PC2_39 56
# define DES_PC2_40 34
# define DES_PC2_41 53
# define DES_PC2_42 46
# define DES_PC2_43 42
# define DES_PC2_44 50
# define DES_PC2_45 36
# define DES_PC2_46 29
# define DES_PC2_47 32

/* How far the two key halves turn left before each round */
# define DES_SHIFT_0 1
# define DES_SHIFT_1 1
# define DES_SHIFT_2 2
# define DES_SHIFT_3 2
# define DES_SHIFT_4 2
# define DES_SHIFT_5 2
# define DES_SHIFT_6 2
# define DES_SHIFT_7 2
# define DES_SHIFT_8 1
# define DES_SHIFT_9 2
# define DES_SHIFT_10 2
# define DES_SHIFT_11 2
# define DES_SHIFT_12 2
# define DES_SHIFT_13 2
# define DES_SHIFT_14 2
# define DES_SHIFT_15 1

/* S-box S1: entry row * 16 + column, row and column from 6 bits */
# define DES_S1_0 14
# define DES_S1_1 4
# define DES_S1_2 13
# define DES_S1_3 1
# define DES_S1_4 2
# define DES_S1_5 15
# define DES_S1_6 11
# define DES_S1_7 8
# define DES_S1_8 3
# define DES_S1_9 10
# define DES_S1_10 6
# define DES_S1_11 12
# define DES_S1_12 5
# define DES_S1_13 9
# define DES_S1_14 0
# define DES_S1_15 7
# define DES_S1_16 0
# define DES_S1_17 15
# define DES_S1_18 7
# define DES_S1_19 4
# define DES_S1_20 14
# define DES_S1_21 2
# define DES_S1_22 13
# define DES_S1_23 1
# define DES_S1_24 10
# define DES_S1_25 6
# define DES_S1_26 12
# define DES_S1_27 11
# define DES_S1_28 9
# define DES_S1_29 5
# define DES_S1_30 3
# define DES_S1_31 8
# define DES_S1_32 4
# define DES_S1_33 1
# define DES_S1_34 14
# define DES_S1_35 8
# define DES_S1_36 13
# define DES_S1_37 6
# define DES_S1_38 2
# define DES_S1_39 11
# define DES_S1_40 15
# define DES_S1_41 12
# define DES_S1_42 9
# define DES_S1_43 7
# define DES_S1_44 3
# define DES_S1_45 10
# define DES_S1_46 5
# define DES_S1_47 0
# define DES_S1_48 15
# define DES_S1_49 12
# define DES_S1_50 8
# define DES_S1_51 2
# define DES_S1_52 4
# define DES_S1_53 9
# define DES_S1_54 1
# define DES_S1_55 7
# define DES_S1_56 5
# define DES_S1_57 11
# define DES_S1_58 3
# define DES_S1_59 14
# define DES_S1_60 10
# define DES_S1_61 0
# define DES_S1_62 6
# define DES_S1_63 13

/* S-box S2: entry row * 16 + column, row and column from 6 bits */
# define DES_S2_0 15
# define DES_S2_1 1
# define DES_S2_2 8
# define DES_S2_3 14
# define DES_S2_4 6
# define DES_S2_5 11
# define DES_S2_6 3
# define DES_S2_7 4
# define DES_S2_8 9
# define DES_S2_9 7
# define DES_S2_10 2
# define DES_S2_11 13
# define DES_S2_12 12
# define DES_S2_13 0
# define DES_S2_14 5
# define DES_S2_15 10
# define DES_S2_16 3
# define DES_S2_17 13
# define DES_S2_18 4
# define DES_S2_19 7
# define DES_S2_20 15
# define DES_S2_21 2
# define DES_S2_22 8
# define DES_S2_23 14
# define DES_S2_24 12
# define DES_S2_25 0
# define DES_S2_26 1
# define DES_S2_27 10
# define DES_S2_28 6
# define DES_S2_29 9
# define DES_S2_30 11
# define DES_S2_31 5
# define DES_S2_32 0
# define DES_S2_33 14
# define DES_S2_34 7
# define DES_S2_35 11
# define DES_S2_36 10
# define DES_S2_37 4
# define DES_S2_38 13
# define DES_S2_39 1
# define DES_S2_40 5
# define DES_S2_41 8
# define DES_S2_42 12
# define DES_S2_43 6
# define DES_S2_44 9
# define DES_S2_45 3
# define DES_S2_46 2
# define DES_S2_47 15
# define DES_S2_48 13
# define DES_S2_49 8
# define DES_S2_50 10
# define DES_S2_51 1
# define DES_S2_52 3
# define DES_S2_53 15
# define DES_S2_54 4
# define DES_S2_55 2
# define DES_S2_56 11
# define DES_S2_57 6
# define DES_S2_58 7
# define DES_S2_59 12
# define DES_S2_60 0
# define DES_S2_61 5
# define DES_S2_62 14
# define DES_S2_63 9

/* S-box S3: entry row * 16 + column, row and column from 6 bits */
# define DES_S3_0 10
# define DES_S3_1 0
# define DES_S3_2 9
# define DES_S3_3 14
# define DES_S3_4 6
# define DES_S3_5 3
# define DES_S3_6 15
# define DES_S3_7 5
# define DES_S3_8 1
# define DES_S3_9 13
# define DES_S3_10 12
# define DES_S3_11 7
# define DES_S3_12 11
# define DES_S3_13 4
# define DES_S3_14 2
# define DES_S3_15 8
# define DES_S3_16 13
# define DES_S3_17 7
# define DES_S3_18 0
# define DES_S3_19 9
# define DES_S3_20 3
# define DES_S3_21 4
# define DES_S3_22 6
# define DES_S3_23 10
# define DES_S3_24 2
# define DES_S3_25 8
# define DES_S3_26 5
# define DES_S3_27 14
# define DES_S3_28 12
# define DES_S3_29 11
# define DES_S3_30 15
# define DES_S3_31 1
# define DES_S3_32 13
# define DES_S3_33 6
# define DES_S3_34 4
# define DES_S3_35 9
# define DES_S3_36 8
# define DES_S3_37 15
# define DES_S3_38 3
# define DES_S3_39 0
# define DES_S3_40 11
# define DES_S3_41 1
# define DES_S3_42 2
# define DES_S3_43 12
# define DES_S3_44 5
# define DES_S3_45 10
# define DES_S3_46 14
# define DES_S3_47 7
# define DES_S3_48 1
# define DES_S3_49 10
# define DES_S3_50 13
# define DES_S3_51 0
# define DES_S3_52 6
# define DES_S3_53 9
# define DES_S3_54 8
# define DES_S3_55 7
# define DES_S3_56 4
# define DES_S3_57 15
# define DES_S3_58 14
# define DES_S3_59 3
# define DES_S3_60 11
# define DES_S3_61 5
# define DES_S3_62 2
# define DES_S3_63 12

/* S-box S4: entry row * 16 + column, row and column from 6 bits */
# define DES_S4_0 7
# define DES_S4_1 13
# define DES_S4_2 14
# define DES_S4_3 3
# define DES_S4_4 0
# define DES_S4_5 6
# define DES_S4_6 9
# define DES_S4_7 10
# define DES_S4_8 1
# define DES_S4_9 2
# define DES_S4_10 8
# define DES_S4_11 5
# define DES_S4_12 11
# define DES_S4_13 12
# define DES_S4_14 4
# define DES_S4_15 15
# define DES_S4_16 13
# define DES_S4_17 8
# define DES_S4_18 11
# define DES_S4_19 5
# define DES_S4_20 6
# define DES_S4_21 15
# define DES_S4_22 0
# define DES_S4_23 3
# define DES_S4_24 4
# define DES_S4_25 7
# define DES_S4_26 2
# define DES_S4_27 12
# define DES_S4_28 1
# define DES_S4_29 10
# define DES_S4_30 14
# define DES_S4_31 9
# define DES_S4_32 10
# define DES_S4_33 6
# define DES_S4_34 9
# define DES_S4_35 0
# define DES_S4_36 12
# define DES_S4_37 11
# define DES_S4_38 7
# define DES_S4_39 13
# define DES_S4_40 15
# define DES_S4_41 1
# define DES_S4_42 3
# define DES_S4_43 14
# define DES_S4_44 5
# define DES_S4_45 2
# define DES_S4_46 8
# define DES_S4_47 4
# define DES_S4_48 3
# define DES_S4_49 15
# define DES_S4_50 0
# define DES_S4_51 6
# define DES_S4_52 10
# define DES_S4_53 1
# define DES_S4_54 13
# define DES_S4_55 8
# define DES_S4_56 9
# define DES_S4_57 4
# define DES_S4_58 5
# define DES_S4_59 11
# define DES_S4_60 12
# define DES_S4_61 7
# define DES_S4_62 2
# define DES_S4_63 14

/* S-box S5: entry row * 16 + column, row and column from 6 bits */
# define DES_S5_0 2
# define DES_S5_1 12
# define DES_S5_2 4
# define DES_S5_3 1
# define DES_S5_4 7
# define DES_S5_5 10
# define DES_S5_6 11
# define DES_S5_7 6
# define DES_S5_8 8
# define DES_S5_9 5
# define DES_S5_10 3
# define DES_S5_11 15
# define DES_S5_12 13
# define DES_S5_13 0
# define DES_S5_14 14
# define DES_S5_15 9
# define DES_S5_16 14
# define DES_S5_17 11
# define DES_S5_18 2
# define DES_S5_19 12
# define DES_S5_20 4
# define DES_S5_21 7
# define DES_S5_22 13
# define DES_S5_23 1
# define DES_S5_24 5
# define DES_S5_25 0
# define DES_S5_26 15
# define DES_S5_27 10
# define DES_S5_28 3
# define DES_S5_29 9
# define DES_S5_30 8
# define DES_S5_31 6
# define DES_S5_32 4
# define DES_S5_33 2
# define DES_S5_34 1
# define DES_S5_35 11
# define DES_S5_36 10
# define DES_S5_37 13
# define DES_S5_38 7
# define DES_S5_39 8
# define DES_S5_40 15
# define DES_S5_41 9
# define DES_S5_42 12
# define DES_S5_43 5
# define DES_S5_44 6
# define DES_S5_45 3
# define DES_S5_46 0
# define DES_S5_47 14
# define DES_S5_48 11
# define DES_S5_49 8
# define DES_S5_50 12
# define DES_S5_51 7
# define DES_S5_52 1
# define DES_S5_53 14
# define DES_S5_54 2
# define DES_S5_55 13
# define DES_S5_56 6
# define DES_S5_57 15
# define DES_S5_58 0
# define DES_S5_59 9
# define DES_S5_60 10
# define DES_S5_61 4
# define DES_S5_62 5
# define DES_S5_63 3

/* S-box S6: entry row * 16 + column, row and column from 6 bits */
# define DES_S6_0 12
# define DES_S6_1 1
# define DES_S6_2 10
# define DES_S6_3 15
# define DES_S6_4 9
# define DES_S6_5 2
# define DES_S6_6 6
# define DES_S6_7 8
# define DES_S6_8 0
# define DES_S6_9 13
# define DES_S6_10 3
# define DES_S6_11 4
# define DES_S6_12 14
# define DES_S6_13 7
# define DES_S6_14 5
# define DES_S6_15 11
# define DES_S6_16 10
# define DES_S6_17 15
# define DES_S6_18 4
# define DES_S6_19 2
# define DES_S6_20 7
# define DES_S6_21 12
# define DES_S6_22 9
# define DES_S6_23 5
# define DES_S6_24 6
# define DES_S6_25 1
# define DES_S6_26 13
# define DES_S6_27 14
# define DES_S6_28 0
# define DES_S6_29 11
# define DES_S6_30 3
# define DES_S6_31 8
# define DES_S6_32 9
# define DES_S6_33 14
# define DES_S6_34 15
# define DES_S6_35 5
# define DES_S6_36 2
# define DES_S6_37 8
# define DES_S6_38 12
# define DES_S6_39 3
# define DES_S6_40 7
# define DES_S6_41 0
# define DES_S6_42 4
# define DES_S6_43 10
# define DES_S6_44 1
# define DES_S6_45 13
# define DES_S6_46 11
# define DES_S6_47 6
# define DES_S6_48 4
# define DES_S6_49 3
# define DES_S6_50 2
# define DES_S6_51 12
# define DES_S6_52 9
# define DES_S6_53 5
# define DES_S6_54 15
# define DES_S6_55 10
# define DES_S6_56 11
# define DES_S6_57 14
# define DES_S6_58 1
# define DES_S6_59 7
# define DES_S6_60 6
# define DES_S6_61 0
# define DES_S6_62 8
# define DES_S6_63 13

/* S-box S7: entry row * 16 + column, row and column from 6 bits */
# define DES_S7_0 4
# define DES_S7_1 11
# define DES_S7_2 2
# define DES_S7_3 14
# define DES_S7_4 15
# define DES_S7_5 0
# define DES_S7_6 8
# define DES_S7_7 13
# define DES_S7_8 3
# define DES_S7_9 12
# define DES_S7_10 9
# define DES_S7_11 7
# define DES_S7_12 5
# define DES_S7_13 10
# define DES_S7_14 6
# define DES_S7_15 1
# define DES_S7_16 13
# define DES_S7_17 0
# define DES_S7_18 11
# define DES_S7_19 7
# define DES_S7_20 4
# define DES_S7_21 9
# define DES_S7_22 1
# define DES_S7_23 10
# define DES_S7_24 14
# define DES_S7_25 3
# define DES_S7_26 5
# define DES_S7_27 12
# define DES_S7_28 2
# define DES_S7_29 15
# define DES_S7_30 8
# define DES_S7_31 6
# define DES_S7_32 1
# define DES_S7_33 4
# define DES_S7_34 11
# define DES_S7_35 13
# define DES_S7_36 12
# define DES_S7_37 3
# define DES_S7_38 7
# define DES_S7_39 14
# define DES_S7_40 10
# define DES_S7_41 15
# define DES_S7_42 6
# define DES_S7_43 8
# define DES_S7_44 0
# define DES_S7_45 5
# define DES_S7_46 9
# define DES_S7_47 2
# define DES_S7_48 6
# define DES_S7_49 11
# define DES_S7_50 13
# define DES_S7_51 8
# define DES_S7_52 1
# define DES_S7_53 4
# define DES_S7_54 10
# define DES_S7_55 7
# define DES_S7_56 9
# define DES_S7_57 5
# define DES_S7_58 0
# define DES_S7_59 15
# define DES_S7_60 14
# define DES_S7_61 2
# define DES_S7_62 3
# define DES_S7_63 12

/* S-box S8: entry row * 16 + column, row and column from 6 bits */
# define DES_S8_0 13
# define DES_S8_1 2
# define DES_S8_2 8
# define DES_S8_3 4
# define DES_S8_4 6
# define DES_S8_5 15
# define DES_S8_6 11
# define DES_S8_7 1
# define DES_S8_8 10
# define DES_S8_9 9
# define DES_S8_10 3
# define DES_S8_11 14
# define DES_S8_12 5
# define DES_S8_13 0
# define DES_S8_14 12
# define DES_S8_15 7
# define DES_S8_16 1
# define DES_S8_17 15
# define DES_S8_18 13
# define DES_S8_19 8
# define DES_S8_20 10
# define DES_S8_21 3
# define DES_S8_22 7
# define DES_S8_23 4
# define DES_S8_24 12
# define DES_S8_25 5
# define DES_S8_26 6
# define DES_S8_27 11
# define DES_S8_28 0
# define DES_S8_29 14
# define DES_S8_30 9
# define DES_S8_31 2
# define DES_S8_32 7
# define DES_S8_33 11
# define DES_S8_34 4
# define DES_S8_35 1
# define DES_S8_36 9
# define DES_S8_37 12
# define DES_S8_38 14
# define DES_S8_39 2
# define DES_S8_40 0
# define DES_S8_41 6
# define DES_S8_42 10
# define DES_S8_43 13
# define DES_S8_44 15
# define DES_S8_45 3
# define DES_S8_46 5
# define DES_S8_47 8
# define DES_S8_48 2
# define DES_S8_49 1
# define DES_S8_50 14
# define DES_S8_51 7
# define DES_S8_52 4
# define DES_S8_53 10
# define DES_S8_54 8
# define DES_S8_55 13
# define DES_S8_56 15
# define DES_S8_57 12
# define DES_S8_58 9
# define DES_S8_59 0
# define DES_S8_60 3
# define DES_S8_61 5
# define DES_S8_62 6
# define DES_S8_63 11

#endif
