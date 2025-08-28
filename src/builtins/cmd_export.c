/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_export.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 15:28:36 by airupert          #+#    #+#             */
/*   Updated: 2025/08/28 20:23:11 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static void	sort_env_array(char **sorted_env, int count)
{
	int		i;
	int		x;
	char	*temp;

	i = 0;
	while (i < count - 1)
	{
		x = i + 1;
		while (x < count)
		{
			if (ft_strcmp(sorted_env[i], sorted_env[x]) > 0)
			{
				temp = sorted_env[i];
				sorted_env[i] = sorted_env[x];
				sorted_env[x] = temp;
			}
			x++;
		}
		i++;
	}
}

static void	print_sorted_env(Command *cmd, char **sorted_env)
{
	int		i;

	i = 0;
	while (sorted_env[i])
	{
		write(cmd->out_fd, "delare -x ", 11);
		write(cmd->out_fd, sorted_env[i], ft_strlen(sorted_env[i]));
		write(cmd->out_fd, "\n", 1);
		i++;
	}
}

static int	handle_no_args(Command *cmd, t_shell *shell)
{
	char	**sorted_env;

	if (!shell->env)
		return ((shell->exit_status = 0), 0);
	sorted_env = copy_env_array(shell);
	if (!sorted_env)
	{
		write_error("minishell: malloc error\n", 1, shell);
		return (1);
	}
	sort_env_array(sorted_env, shell->env_count);
	print_sorted_env(cmd, sorted_env);
	free(sorted_env);
	return (0);
}

static int	process_export_args(Command *cmd, t_shell *shell)
{
	int	i;
	int	error;

	i = 1;
	error = 0;
	while (cmd->args[i])
	{
		if (!is_valid_identifier(cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: export: ", 20);
			write(STDERR_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
			write(STDERR_FILENO, "`:not a valid identifier\n", 25);
			error = 1;
		}
		else if (!update_env(shell, cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: export: \
				failed to update enviornment", 48);
			error = 1;
		}
		i++;
	}
	return (error);
}

int	cmd_export(Command *cmd, t_shell *shell)
{
	int		error;

	if (!shell || !cmd)
	{
		write(STDERR_FILENO, "minishell: export: invalid context\n", 34);
		return (1);
	}
	if (!cmd->args[1])
		return (handle_no_args(cmd, shell));
	error = process_export_args(cmd, shell);
	shell->exit_status = error;
	return (error);
}
