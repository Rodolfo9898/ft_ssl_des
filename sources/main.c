/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ft_ssl_des.h"

/*
** Without a command: the usage (make), or the commands of the standard input,
** one per line (make bonus, see ft_ssl_md5's bonus/stdin_bonus.c).
*/
int	main(int argc, char **argv)
{
	if (argc < MIN_ARGC)
	{
		if (BONUS_MODE)
			return (run_stdin_commands());
		ft_putendl_fd(MSG_USAGE, STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	return (execute_command_argv(argc - 1, argv + 1));
}
