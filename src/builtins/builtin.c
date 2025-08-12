/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:10 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/12 10:12:32 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int is_builtin(const char *cmd)
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

static int	cmd_exit(Command *cmd, t_shell *shell)
{
	shell->exit_flag = 1;
	if (cmd->args[1])
	{
		if (cmd->args[2] || !is_numeric(cmd->args[1]))
		{
			write(STDERR_FILENO, "minishell: exit: numeric argument required\n", 43);
			shell->exit_status = 2;
			return (2);
		}
		shell->exit_status = ft_atoi(cmd->args[1]) % 256;
	}
	else
		shell->exit_status = 0;
	return (shell->exit_status);
}

static int cmd_env(Command *cmd, t_shell *shell)
{
    int i;

    if (!shell || !shell->env)
    {
        write(STDERR_FILENO, "minishell: env: invalid identifier\n", 31);
        shell->exit_status = 1;
        return (1);
    }
    if (cmd->args[1])
    {
        write(STDERR_FILENO, "minishell: env: no options or arguments allowed\n", 48);
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
	int	error = 0;
	
	if (!cmd->args[1]) // unset with no arguments is valid in bash - just does nothing
		return ((shell->exit_status = 0), 0);
	i = 1;
	while (cmd->args[i])
	{		// Check if it's a valid identifier first
		if (!is_valid_identifier(cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: unset: `", 19);
			write(STDERR_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
			write(STDERR_FILENO, "': not a valid identifier\n", 26);
			error = 1;
		}
		else	// remove_env now returns 0 on success, 1 on error
			remove_env(shell, cmd->args[i]);
		i++;
	}
	shell->exit_status = error;
	return (error);
}

static int	cmd_export(Command *cmd, t_shell *shell)
{
	int	i;
	int	error;

	error = 0;
	if (!shell || !cmd)
		return ((write(STDERR_FILENO, "minishell: export: invalid context\n", 34)), 1);
	if (!cmd->args[1])
	{
		if (!shell->env)
			return ((shell->exit_status = 0), 0);
		i = 0;
		while (shell->env[i])
		{
			write(cmd->out_fd, "declare -x ", 11); //debug?
			write(cmd->out_fd, shell->env[i], ft_strlen(shell->env[i]));
			write(cmd->out_fd, "\n", 1);
			i++;
		}
		shell->exit_status = 0;
		return (0);
	}
	i = 1;
	while (cmd->args[i])
	{
		if (!is_valid_identifier(cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: export: `", 20);
			write(STDERR_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
			write(STDERR_FILENO, "`:not a valid identifier\n", 25);
			error = 1;
		}
		else if (!update_env(shell, cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: export: failed to update environment\n", 48);
			error = 1;
		}
		i++;
	}
	shell->exit_status = error;
	return (error);
}

static int	cmd_pwd(Command *cmd, t_shell *shell)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		write(cmd->out_fd, cwd, ft_strlen(cwd));
		write(cmd->out_fd, "\n", 1);
		shell->exit_status = 0;
	}
	else
	{
		write(STDERR_FILENO, "minishell: pwd: error retrieving current directory\n", 50);
		shell->exit_status = 1;
	}
	return (shell->exit_status);
}

static int	cmd_cd(Command *cmd, t_shell *shell)
{
	char	*path;
	char	cwd[1024];
	char	*oldpwd;
	char	*oldpwd_str;
	char	*pwd_str;
	
	path = cmd->args[1];
	oldpwd = getcwd(cwd, sizeof(cwd));
	if (!path)
		path = getenv("HOME");
	if (!path || chdir(path) != 0)
	{
		write(STDERR_FILENO, "minishell: cd: invalid path\n", 28);
		shell->exit_status = 1;
		return (1);
	}
	if (oldpwd)
	{
		oldpwd_str = ft_strjoin("OLDPWD=", oldpwd);
		if (oldpwd_str)
		{
			update_env(shell, oldpwd_str);
			free(oldpwd_str);
		}
	}
	if (getcwd(cwd, sizeof(cwd)))
	{
		pwd_str = ft_strjoin("PWD=", cwd);
		if (pwd_str)
		{
			update_env(shell, pwd_str);
			free(pwd_str);
		}
	}
	shell->exit_status = 0;
	return (0);
}

static int	cmd_echo(Command *cmd, t_shell *shell)
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
	while (cmd->args[i])
	{
		if (write(cmd->out_fd, cmd->args[i], ft_strlen(cmd->args[i])) == -1)
		{
			write(STDERR_FILENO, "minishell: echo: write error\n", 29);
			shell->exit_status = 1;
			return (1);
		}
		if (cmd->args[i + 1])
		{
			if (write(cmd->out_fd, " ", 1) == -1)
			{
				write(STDERR_FILENO, "minishell: echo: write error\n", 29);
				shell->exit_status = 1;
				return (1);
			}
		}
		i++;
	}
	if (!no_newline)
	{
		if (write(cmd->out_fd, "\n", 1) == -1)
		{
			write(STDERR_FILENO, "minishell: echo: write error\n", 29);
			shell->exit_status = 1;
			return (1);
		}
	}
	shell->exit_status = 0;
	return (0);
}

// Execute built-in command
int exec_builtin(Command *cmd, t_shell *shell)
{
	if (!cmd || !shell)
		return (1);
    if (ft_strcmp(cmd->cmd, "echo") == 0) // echo builtin + "-n" option
		return (cmd_echo(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "cd") == 0)
		return (cmd_cd(cmd, shell));
	else if (ft_strcmp(cmd->cmd, "pwd") == 0)
		return(cmd_pwd(cmd, shell));
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
