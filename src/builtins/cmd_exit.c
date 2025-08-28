/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exit.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 20:28:50 by airupert          #+#    #+#             */
/*   Updated: 2025/08/28 21:07:43 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static int	handle_numeric_arg(Command *cmd, t_shell *shell, pid_t current_pid)
{
	if (cmd->args[2])
	{
		write(STDERR_FILENO, "minishell: exit: too many arguments\n", 37);
		shell->exit_status = 1;
		return (1);
	}
	if (!is_numeric(cmd->args[1]))
	{
		write(STDERR_FILENO, "minishell: exit: ", 17);
		write(STDERR_FILENO, cmd->args[1], ft_strlen(cmd->args[1]));
		write(STDERR_FILENO, ": numeric argument required\n", 28);
		shell->exit_status = 2;
		if (current_pid != shell->pid)
			exit(2);
		return (2);
	}
	shell->exit_status = ft_atoi(cmd->args[1]) % 256;
	return (0);
}

int	cmd_exit(Command *cmd, t_shell *shell)
{
	pid_t	current_pid;
	int		result;

	current_pid = getpid();
	if (!shell || !cmd)
	{
		write_error("minishell: exit: invalid context\n", 1, shell);
		if (current_pid != shell->pid)
			exit(1);
		return (1);
	}
	if (current_pid == shell->pid)
		shell->exit_flag = 1;
	while (cmd->args[1])
	{
		result = handle_numeric_arg(cmd, shell, current_pid);
		if (result != 0)
			return (result);
		break ;
	}
	if (!cmd->args[1])
		shell->exit_status = 0;
	if (current_pid != shell->pid)
		exit(shell->exit_status);
	return (shell->exit_status);
}
