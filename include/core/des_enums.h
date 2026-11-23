/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_enums.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DES_ENUMS_H
# define DES_ENUMS_H

# include "des_defines.h"

/* The block cipher modes: ECB and CBC, and OFB in the bonus */
typedef enum e_des_mode
{
	DES_MODE_ECB,
	DES_MODE_CBC,
	DES_MODE_OFB
}	t_des_mode;

/* What a DES block function does with its block */
typedef enum e_direction
{
	DIRECTION_ENCRYPT,
	DIRECTION_DECRYPT
}	t_direction;

/* The three keys of Triple DES, in the order E-D-E uses them */
typedef enum e_des3_key
{
	DES3_FIRST,
	DES3_SECOND,
	DES3_THIRD
}	t_des3_key;

#endif
