/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_echo.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 15:27:16 by airupert          #+#    #+#             */
/*   Updated: 2025/08/27 16:10:01 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	write_with_error_check(int fd, const char *str, \
	size_t len, t_shell *shell)
{
	if (write(fd, str, len) == -1)
	{
		write_error("minishell: echo: write error\n", 1, shell);
		return (1);
	}
	return (0);
}

static int	write_args(Command *cmd, int start_idx, t_shell *shell)
{
	int	i;

	i = start_idx;
	while (cmd->args[i])
	{
		if (write_with_error_check(cmd->out_fd, cmd->args[i], \
				ft_strlen(cmd->args[i]), shell))
			return (1);
		if (cmd->args[i + 1])
		{
			if (write_with_error_check(cmd->out_fd, " ", 1, shell))
				return (1);
		}
		i++;
	}
	return (0);
}

int	cmd_echo(Command *cmd, t_shell *shell)
{
	int	i;
	int	no_newline;

	i = 1;
	no_newline = 0;
	while (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		no_newline = 1;
		i++;
	}
	if (write_args(cmd, 1, shell))
		return (1);
	if (!no_newline)
	{
		if (write_with_error_check(cmd->out_fd, "\n", 1, shell))
			return (1);
	}
	shell->exit_status = 0;
	return (0);
}
