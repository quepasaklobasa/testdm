/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:36:08 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/06 20:56:11 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Get path for external command
char *get_command_path(const char *cmd, t_shell *shell)
{
    char	*path;
	char	*paths;
	char	*full_path;
	char	*path_env;
	char	*dir;

	if (!cmd)
	{
		write(2, "minishell: get_command_path: null command\n", 41);
		return (NULL);
	}
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	path_env = get_env_value(shell->env, "PATH");
	if (!path_env)
	{
		// fallback to /bin/ if PATH is unset
		full_path = ft_strjoin("/bin/", cmd);
		if (!full_path)
		{
			write(2, "minishell: malloc: cannot allocate memory\n", 41);
			return (NULL);
		}
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		return (NULL);
	}
	paths = ft_strdup(path_env);
	if (!paths)
	{
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		return(NULL);
	}
	dir = ft_strtok(paths, ":");
	while (dir)
	{
		full_path = ft_strjoin(dir, "/");
		path = ft_strjoin(full_path, cmd);
		free(full_path);
		if (!path)
		{
			free(paths);
			write(2, "minshell: malloc: cannot allocate memory\n", 41);
			return (NULL);
		}
		if(access(path, X_OK) == 0)
		{
			free(paths);
			return (path);
		}
		free(path);
		dir = ft_strtok(NULL, ":");
	}
	free(paths);
	return (NULL);
}

// Execute external command
int exec_external(Command *cmd, t_shell *shell)
{
    char *path;

	if (!cmd || !cmd->cmd || !cmd->args)
	{
		write(STDERR_FILENO, "minishell: exec_external: invalid command\n", 41);
		shell->exit_status = 1;
		return (1);
	}
    path = get_command_path(cmd->cmd, shell);
    if (!path)
    {
		write(STDERR_FILENO, "minishell: command not found: ", 30);
		write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
		write(STDERR_FILENO, "\n", 1);
        shell->exit_status = 127;
        return (127);
    }
	execve(path, cmd->args, shell->env);
	write(STDERR_FILENO, "minishell: command not found: ", 30);
	write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
	write(STDERR_FILENO, "\n", 1);
    free(path);
    shell->exit_status = 127;
	return (127);
}
