/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_registry.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rperez-t <rperez-t@student.42belgium.be    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 23:07:38 by rperez-t          #+#    #+#             */
/*   Updated: 2026/03/16 21:42:44 by rperez-t         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_ssl.h"

static const t_command	g_commands[] = {
{"md5", COMMAND_GROUP_MESSAGE_DIGEST, run_digest_command,
	(const void *)get_md5_algorithm},
{"sha256", COMMAND_GROUP_MESSAGE_DIGEST, run_digest_command,
	(const void *)get_sha256_algorithm},
{"whirlpool", COMMAND_GROUP_MESSAGE_DIGEST, run_digest_command,
	(const void *)get_whirlpool_algorithm},
{"base64", COMMAND_GROUP_CIPHER, run_base64_command, NULL},
{"des", COMMAND_GROUP_CIPHER, run_des_command, NULL},
{"des-ecb", COMMAND_GROUP_CIPHER, run_des_command, NULL},
{"des-cbc", COMMAND_GROUP_CIPHER, run_des_command, NULL},
{"des-ofb", COMMAND_GROUP_CIPHER, run_des_command, NULL},
{"des3", COMMAND_GROUP_CIPHER, run_des_command, NULL},
{"des3-ecb", COMMAND_GROUP_CIPHER, run_des_command, NULL},
{"des3-cbc", COMMAND_GROUP_CIPHER, run_des_command, NULL},
{"des3-ofb", COMMAND_GROUP_CIPHER, run_des_command, NULL}
};

const t_command	*get_command_table(size_t *count)
{
	if (count != NULL)
		*count = sizeof(g_commands) / sizeof(g_commands[0]);
	return (g_commands);
}

const t_command	*find_command(const char *name)
{
	size_t			count;
	size_t			index;
	const t_command	*commands;

	commands = get_command_table(&count);
	index = 0;
	while (index < count)
	{
		if (ft_is_streq(commands[index].name, name))
			return (&commands[index]);
		index++;
	}
	return (NULL);
}

int	execute_command_argv(int argc, char **argv)
{
	const t_command	*command;

	if (argc <= 0)
		return (0);
	command = find_command(argv[0]);
	if (command == NULL)
	{
		print_invalid_command(argv[0]);
		return (1);
	}
	return (command->handler(command, argc - 1, argv + 1));
}
