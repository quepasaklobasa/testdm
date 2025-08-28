/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:10 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 21:14:04 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	is_builtin(const char *cmd)
{
	if (!cmd)
		return (0);
	if (ft_strcmp(cmd, "echo") == 0 || ft_strcmp(cmd, "cd") == 0 \
		|| ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "export") == 0 \
		|| ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "env") == 0 \
		|| ft_strcmp(cmd, "exit") == 0)
		return (1);
	return (0);
}

static int	cmd_env(Command *cmd, t_shell *shell)
{
	int		i;

	if (!shell || !shell->env)
	{
		write(STDERR_FILENO, "minishell: env: invalid identifier\n", 31);
		shell->exit_status = 1;
		return (1);
	}
	if (cmd->args[1])
	{
		write(STDERR_FILENO, "minishell: env: no \
			options or arguments allowed\n", 48);
		shell->exit_status = 1;
		return (1);
	}
	i = 0;
	while (shell->env[i])
	{
		write(cmd->out_fd, shell->env[i], ft_strlen(shell->env[i]));
		write(cmd->out_fd, "\n", 1);
		i++;
	}
	shell->exit_status = 0;
	return (0);
}

static int	cmd_unset(Command *cmd, t_shell *shell)
{
	int	i;
	int	error;

	error = 0;
	if (!cmd->args[1])
		return ((shell->exit_status = 0), 0);
	i = 1;
	while (cmd->args[i])
	{
		if (!is_valid_identifier(cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: unset: `", 19);
			write(STDERR_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
			write(STDERR_FILENO, "': not a valid identifier\n", 26);
			error = 1;
		}
		else
		{
			if (remove_env(shell, cmd->args[i]) != 0)
				error = 1;
		}
		i++;
	}
	shell->exit_status = error;
	return (error);
}

// Execute built-in command
int	exec_builtin(Command *cmd, t_shell *shell)
{
	if (!cmd || !shell)
		return (1);
	if (ft_strcmp(cmd->cmd, "echo") == 0)
		return (cmd_echo(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "cd") == 0)
		return (cmd_cd(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "pwd") == 0)
		return (cmd_pwd(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "export") == 0)
		return (cmd_export(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "unset") == 0)
		return (cmd_unset(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "env") == 0)
		return (cmd_env(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "exit") == 0)
		return (cmd_exit(cmd, shell));
	return (1);
}
