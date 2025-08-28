/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:36:08 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:28:35 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Get path for external command
int get_command_path(const char *cmd, t_shell *shell, char *path, size_t path_size)
{
	char *path_env;
	size_t dir_len;
	size_t cmd_len;
	size_t i;
	size_t start;
	int j;
	
	if (!cmd || !path || path_size < 2)
	{
		write(STDERR_FILENO, "minishell: get_command_path: invalid input\n", 43);
		return (0);
	}
	
	path[0] = '\0';
	cmd_len = ft_strlen(cmd);
	
	// If cmd contains '/' or is absolute path, check directly
	if (ft_strchr(cmd, '/') || access(cmd, X_OK) == 0)
	{
		if (cmd_len >= path_size)
		{
			write(STDERR_FILENO, "minishell: get_command_path: path too long\n", 44);
			return (0);
		}
		ft_strlcpy(path, cmd, path_size);
		return (1);
	}
	
	path_env = get_env_value(shell->env, "PATH");
	if (!path_env)
	{
		const char *fallback_dirs[] = {"/bin/", "/usr/bin/", "/usr/local/bin/", NULL};
		j = 0;
		while (fallback_dirs[j] != NULL)
		{
			size_t fallback_len = ft_strlen(fallback_dirs[j]);
			// Check: dir_len + cmd_len + null terminator
			if (fallback_len + cmd_len + 1 > path_size)
			{
				write(STDERR_FILENO, "minishell: get_command_path: path too long\n", 44);
				return (0);
			}
			ft_strlcpy(path, fallback_dirs[j], path_size);
			ft_strlcat(path, cmd, path_size);
			if (access(path, X_OK) == 0)
				return (1);
			path[0] = '\0';
			j++;
		}
		return (0);
	}
	
	i = 0;
	start = 0;
	
	while (path_env[i] != '\0')
	{
		if (path_env[i] == ':' || path_env[i + 1] == '\0')
		{
			// Handle last directory correctly
			if (path_env[i + 1] == '\0' && path_env[i] != ':')
				dir_len = i + 1 - start;
			else
				dir_len = i - start;
			
			// Skip empty directories
			if (dir_len == 0)
			{
				start = i + 1;
				i++;
				continue;
			}
			// Check: dir_len + "/" + cmd_len + null terminator
			if (dir_len + 1 + cmd_len + 1 > path_size)
			{
				write(STDERR_FILENO, "minishell: get_command_path: path too long\n", 44);
				return (0);
			}
			// Copy directory path
			ft_strlcpy(path, path_env + start, dir_len + 1);
			// Add slash if directory doesn't end with one
			if (dir_len > 0 && path[dir_len - 1] != '/')
			{
				path[dir_len] = '/';
				path[dir_len + 1] = '\0';
			}
			ft_strlcat(path, cmd, path_size);
			if (access(path, X_OK) == 0)
				return (1);
			start = i + 1;
			path[0] = '\0';
		}
		i++;
	}
	
	return (0);
}

// Execute external command
int exec_external(Command *cmd, t_shell *shell)
{
	pid_t pid;
	int status;
	char path[1024];

	if (!cmd || !cmd->cmd || !cmd->args)
	{
		write(STDERR_FILENO, "minishell: exec_external: invalid command\n", 41);
		shell->exit_status = 1;
		return 1;
	}
	
	if (!get_command_path(cmd->cmd, shell, path, sizeof(path)))
	{
		write(STDERR_FILENO, "minishell: command not found: ", 30);
		write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
		write(STDERR_FILENO, "\n", 1);
		shell->exit_status = 127;
		return 127;
	}
	
	pid = fork();
	if (pid == -1)
	{
		write(STDERR_FILENO, "minishell: fork error\n", 22);
		shell->exit_status = 1;
		return 1;
	}
	
	if (pid == 0)
	{
		setup_fds(cmd, shell);
		execve(path, cmd->args, shell->env);
		write(STDERR_FILENO, "minishell: execve failed: ", 26);
		write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
		write(STDERR_FILENO, "\n", 1);
		exit(127);
	}
	
	waitpid(pid, &status, 0);
	
	if (WIFEXITED(status))
	{
		shell->exit_status = WEXITSTATUS(status);
		return shell->exit_status;
	}
	if (WIFSIGNALED(status))
	{
		shell->exit_status = 128 + WTERMSIG(status);
		return shell->exit_status;
	}
	shell->exit_status = 1;
	return 1;
}