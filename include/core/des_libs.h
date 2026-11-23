/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   des_libs.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DES_LIBS_H
# define DES_LIBS_H

# include <stddef.h>
# include <stdint.h>
# include <errno.h>
# include <fcntl.h>
# include <stdlib.h>
# include <unistd.h>
# include "../../libs/libft/include/libft.h"
# include "../../libs/ft_ssl_md5/include/ft_ssl_md5.h"

/*
** getpass() reads the password without showing it (ft_ssl_des.pdf p.6). It is
** in <unistd.h>, which hides it under -std=c99: this is its prototype.
*/
char	*getpass(const char *prompt);

#endif
