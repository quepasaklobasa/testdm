/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_cd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 15:25:35 by airupert          #+#    #+#             */
/*   Updated: 2025/08/27 15:46:36 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// helper function to resolve target path
static char	*cd_path(Command *cmd, t_shell *shell)
{
	char	*path;

	path = cmd->args[1];
	if (!path)
	{
		path = get_env_value(shell->env, "HOME");
		if (!path)
			write_error("minishell: cd: HOME not set\n", 1, shell);
		return (path);
	}
	if (ft_strcmp(path, "-") == 0)
	{
		path = get_env_value(shell->env, "OLDPWD");
		if (!path)
		{
			write_error("minishell: cd: OLDPWD nort set\n", 1, shell);
			return (NULL);
		}
		write(cmd->out_fd, path, ft_strlen(path));
		write(cmd->out_fd, "\n", 1);
	}
	return (path);
}

static int	update_oldpwd(t_shell *shell, char *oldpwd)
{
	char	*oldpwd_str;
	int		result;

	result = 0;
	while (oldpwd)
	{
		oldpwd_str = ft_strjoin("OLDPWD=", oldpwd);
		if (oldpwd_str)
		{
			result = update_env(shell, oldpwd_str);
			free(oldpwd_str);
		}
		break ;
	}
	return (result);
}

static int	update_pwd(t_shell *shell)
{
	char	cwd[1024];
	char	*pwd_str;
	int		success;

	success = 0;
	while (getcwd(cwd, sizeof(cwd)))
	{
		pwd_str = ft_strjoin("PWD=", cwd);
		if (pwd_str)
		{
			update_env(shell, pwd_str);
			free(pwd_str);
			success = 1;
		}
		break ;
	}
	if (!success)
	{
		write(STDERR_FILENO, "minishell: cd: getcwd error\n", 28);
		shell->exit_status = 1;
		return (1);
	}
	return (0);
}

int	cmd_cd(Command *cmd, t_shell *shell)
{
	char	*path;
	char	cwd[1024];
	char	*oldpwd;

	oldpwd = getcwd(cwd, sizeof(cmd));
	path = cd_path(cmd, shell);
	while (path && shell->exit_status == 0)
	{
		if (chdir(path) != 0)
		{
			write(STDERR_FILENO, "minishell: cd: ", 15);
			write(STDERR_FILENO, path, ft_strlen(path));
			write(STDERR_FILENO, " No such file or directory\n", 28);
			shell->exit_status = 1;
			return (1);
		}
		break ;
	}
	if (shell->exit_status != 0)
		return (1);
	update_oldpwd(shell, oldpwd);
	if (update_pwd(shell) != 0)
		return (1);
	shell->exit_status = 0;
	return (0);
}
