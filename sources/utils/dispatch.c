/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dispatch.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/23 15:00:00 by rperez-t          #+#    #+#             */
/*   Updated: 2026/11/23 15:00:00 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/ft_ssl_des.h"

/*
** The command table: one row per command, its name, its family and the
** function that runs it. md5 and sha256 (whirlpool in the bonus) come from
** ft_ssl_md5, the cipher commands from this project. make bonus adds the
** other modes and Triple DES (ft_ssl_des.pdf p.13).
*/
#if BONUS_MODE

static const t_command	g_commands[] = {
{MD5_NAME, COMMAND_GROUP_MESSAGE_DIGEST, run_md5},
{SHA256_NAME, COMMAND_GROUP_MESSAGE_DIGEST, run_sha256},
{WHIRLPOOL_NAME, COMMAND_GROUP_MESSAGE_DIGEST, run_whirlpool},
{BASE64_NAME, COMMAND_GROUP_CIPHER, run_base64},
{DES_NAME, COMMAND_GROUP_CIPHER, run_des},
{DES_ECB_NAME, COMMAND_GROUP_CIPHER, run_des_ecb},
{DES_CBC_NAME, COMMAND_GROUP_CIPHER, run_des_cbc},
{DES_OFB_NAME, COMMAND_GROUP_CIPHER, run_des_ofb},
{DES3_NAME, COMMAND_GROUP_CIPHER, run_des3},
{DES3_ECB_NAME, COMMAND_GROUP_CIPHER, run_des3_ecb},
{DES3_CBC_NAME, COMMAND_GROUP_CIPHER, run_des3_cbc},
{DES3_OFB_NAME, COMMAND_GROUP_CIPHER, run_des3_ofb}
};

#else

static const t_command	g_commands[] = {
{MD5_NAME, COMMAND_GROUP_MESSAGE_DIGEST, run_md5},
{SHA256_NAME, COMMAND_GROUP_MESSAGE_DIGEST, run_sha256},
{BASE64_NAME, COMMAND_GROUP_CIPHER, run_base64},
{DES_NAME, COMMAND_GROUP_CIPHER, run_des},
{DES_ECB_NAME, COMMAND_GROUP_CIPHER, run_des_ecb},
{DES_CBC_NAME, COMMAND_GROUP_CIPHER, run_des_cbc}
};

#endif

/* The title of a family, then its commands, one per line */
static void	print_group(t_command_group group, char *title)
{
	size_t	index;

	ft_putendl_fd(title, STDERR_FILENO);
	index = 0;
	while (index < sizeof(g_commands) / sizeof(g_commands[0]))
	{
		if (g_commands[index].group == group)
			ft_putendl_fd(g_commands[index].name, STDERR_FILENO);
		index++;
	}
}

static const t_command	*find_command(const char *name)
{
	size_t	index;

	index = 0;
	while (index < sizeof(g_commands) / sizeof(g_commands[0]))
	{
		if (flag_is(name, g_commands[index].name))
			return (&g_commands[index]);
		index++;
	}
	return (NULL);
}

/*
** argv[0] is the command: the arguments after it go to its function. An
** unknown one prints the error and the commands as the subject shows them.
*/
int	execute_command_argv(int argc, char **argv)
{
	const t_command	*command;

	if (argc <= 0)
		return (EXIT_SUCCESS);
	command = find_command(argv[0]);
	if (command == NULL)
	{
		ft_putstr_fd(MSG_INVALID_START, STDERR_FILENO);
		ft_putstr_fd(argv[0], STDERR_FILENO);
		ft_putendl_fd(MSG_INVALID_END, STDERR_FILENO);
		print_group(COMMAND_GROUP_STANDARD, MSG_STANDARD);
		print_group(COMMAND_GROUP_MESSAGE_DIGEST, MSG_DIGEST);
		print_group(COMMAND_GROUP_CIPHER, MSG_CIPHER);
		return (EXIT_FAILURE);
	}
	return (command->handler(argc - 1, argv + 1));
}
