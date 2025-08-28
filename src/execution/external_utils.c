/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:05:46 by airupert          #+#    #+#             */
/*   Updated: 2025/08/19 20:45:52 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// extra util function used in get_command_path()
char	*ft_strtok(char *str, const char *delim)
{
	static char *next_token = NULL;
	char		*token;
	const char	*d;
	char		*start;

	if (str != NULL)
		next_token = str;
	else if (next_token == NULL || *next_token == '\0')
		return (NULL);
	// skip leading delimiters
	start = next_token;
	while(*start)
	{
		d = delim;
		while (*d)
		{
			if (*start == *d)
			{
				start++;
				break ;
			}
			d++;
		}
		if (*d == '\0')
			break ;
	}
	// if end of string, no more tokens
	if (*start == '\0')
	{
		next_token = NULL;
		return (NULL);
	}
	// set the token start
	token = start;
	// find the end of the token
	while (*next_token)
	{
		d = delim;
		while (*d)
		{
			if (*next_token == *d)
			{
				*next_token = '\0';
				next_token++;
				return (token);
			}
			d++;
		}
		next_token++;
	}
	// if reach here, this is last token
	if (*token)
		return (token);
	next_token = NULL;
	return (NULL);
}

// to find executable paths
// if no path, check if cmd is executable
// split PATH int dirs array
// while loop through dirs, build and check each full path
// free everything 
char	*find_executable(const char *cmd, char **env)
{
	char	*path_env;
	char	**dirs;
	char	*full_path;
	char	*path;
	int		i;

	if (ft_strchr(cmd, '/'))
	{
		if (access(cmd, X_OK) == 0)
			return (ft_strdup(cmd));
		return (NULL);
	}
	path_env = get_env_value(env, "PATH");
	if (!path_env)
		return (NULL);
	dirs = ft_split(path_env, ':');
	if (!dirs)
		return (NULL);
	i = 0;
	while (dirs[i])
	{
		full_path = ft_strjoin(dirs[i], "/");
		if (!full_path)
		{
			i = 0;
			while (dirs[i])
				free(dirs[i++]);
			free(dirs);
			return (NULL);
		}
		path = ft_strjoin(full_path, cmd);
		free(full_path);
		if (!path)
		{
			i = 0;
			while (dirs[i])
				free(dirs[i++]);
			free(dirs);
			return (path);
		}
		if (access(path, X_OK) == 0)
		{
			i = 0;
			while (dirs[i])
				free(dirs[i++]);
			free(dirs);
			return (path);
		}
		free(path);
		i++;
	}
	i = 0;
	while (dirs[i])
		free(dirs[i++]);
	free(dirs);
	return (NULL);
}

// call find_executable to get path
// fork: child resets signals and calls execve (exits on failure)
// Parent: Waits with waitpid, extracts exit status (handles normal exit or signals like SIGINT)
int		execute_external(Command *cmd, t_shell *shell)
{
	pid_t	pid;
	char	*path;
	int		status;

	path = find_executable(cmd->cmd, shell->env);
	if (!path)
	{
		write(STDERR_FILENO, "minishell: ", 11);
		write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
		write(STDERR_FILENO, ": command not found\n", 20);
		shell->exit_status = 127;
		return (127);
	}
	pid = fork();
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execve(path, cmd->args, shell->env);
		write(STDERR_FILENO, "minishell: ", 11);
		write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
		write(STDERR_FILENO, ": execve failed\n", 16);
		free(path);
		exit(126);
	}
	free(path);
	waitpid(pid, &status, 0);
	if (WIFEXITED(status))
		shell->exit_status = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		shell->exit_status = 128 + WTERMSIG(status);
	return (shell->exit_status);
}
