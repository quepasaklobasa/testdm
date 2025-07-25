/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:10 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/21 16:56:13 by airupert         ###   ########.fr       */
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

static int	cmd_exit(Command *cmd, t_context *ctx, t_shell *shell)
{
	shell->exit_flag = 1;
	if (cmd->args[1])
	{
		if (cmd->args[2] || !is_numeric(cmd->args[1]))
		{
			write(STDERR_FILENO, "minishell: exit: numeric argument required\n", 43);
			ctx->exit_status = 2;
			return (2);
		}
		ctx->exit_status = ft_atoi(cmd->args[1]) % 256;
	}
	else
		ctx->exit_status = 0;
	return (ctx->exit_status);
}

static int	cmd_env(Command *cmd, t_context *ctx)
{
	int	i;

	if (!ctx)
	{
		write(STDERR_FILENO, "minishell: env: invalid identifier\n", 31);
		return (1);
	}
	if (cmd->args[1])
	{
		write(STDERR_FILENO, "minishell: env: no options or arguments allowed\n", 48);
		ctx->exit_status = 1;
		return (1);
	}
	i = 0;
	if (!ctx->env)
	{
		ctx->exit_status = 0;
		return (0);
	}
	while (ctx->env[i])
	{
		write(cmd->out_fd, ctx->env[i], ft_strlen(ctx->env[i]));
		write(cmd->out_fd, "\n", 1);
		i++;
	}
	ctx->exit_status = 0;
	return (0);
}

static int	cmd_unset(Command *cmd, t_context *ctx)
{
	int	i;
	int	error;

	i = 1;
	while (cmd->args[i])
	{
		if (remove_env(ctx, cmd->args[i])) // builtin helper function
		{
			write(STDERR_FILENO, "minishell: unset: invalid identifier\n", 37);
			ctx->exit_status = 1;
			error = 1;
		}
		i++;
	}
	ctx->exit_status = error;
	return (error);
}

static int	cmd_export(Command *cmd, t_context *ctx)
{
	int	i;
	int	error;

	error = 0;
	if (!ctx || !cmd)
	{
		write(STDERR_FILENO, "minishell: export: invalid context\n", 34);
		if (ctx)
			ctx->exit_status = 1;
		return (1);
	}
	if (!cmd->args[1])
	{
		if (!ctx->env)
		{
			ctx->exit_status = 0;
			return (0);
		}
		i = 0;
		while (ctx->env[i])
		{
			write(cmd->out_fd, ctx->env[i], ft_strlen(ctx->env[i]));
			write(cmd->out_fd, "\n", 1);
			i++;
		}
		ctx->exit_status = 0;
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
		else if (!update_env(ctx, cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: export: failed to update environment\n", 48);
			error = 1;
		}
		i++;
	}
	ctx->exit_status = error;
	return (error);
}

static int	cmd_pwd(Command *cmd, t_context *ctx)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		write(cmd->out_fd, cwd, ft_strlen(cwd));
		write(cmd->out_fd, "\n", 1);
		ctx->exit_status = 0;
	}
	else
	{
		write(STDERR_FILENO, "minishell: pwd: error retrieving current directory\n", 50);
		ctx->exit_status = 1;
	}
	return (ctx->exit_status);
}

static int	cmd_cd(Command *cmd, t_context *ctx)
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
		ctx->exit_status = 1;
		return (1);
	}
	if (oldpwd)
	{
		oldpwd_str = ft_strjoin("OLDPWD=", oldpwd);
		if (oldpwd_str)
		{
			update_env(ctx, oldpwd_str);
			free(oldpwd_str);
		}
	}
	if (getcwd(cwd, sizeof(cwd)))
	{
		pwd_str = ft_strjoin("PWD=", cwd);
		if (pwd_str)
		{
			update_env(ctx, pwd_str);
			free(pwd_str);
		}
	}
	ctx->exit_status = 0;
	return (0);
}

static int	cmd_echo(Command *cmd, t_context *ctx)
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
			ctx->exit_status = 1;
			return (1);
		}
		if (cmd->args[i + 1])
		{
			if (write(cmd->out_fd, " ", 1) == -1)
			{
				write(STDERR_FILENO, "minishell: echo: write error\n", 29);
				ctx->exit_status = 1;
				return (1);
			}
		}
		i++;
	}
	if (!no_newline)
	{
		if (write(cmd->out_fd, "\n", 1) == -1)
		{
			write(STDERR_FILENO, "mimshell: echo: write error \n", 29);
			ctx->exit_status = 1;
			return (1);
		}
	}
	ctx->exit_status = 0;
	return (0);
}

// Execute built-in command
int exec_builtin(Command *cmd, t_context *ctx, t_shell *shell) // t_shell added for exit flag access
{
	if (!cmd || !ctx || !shell)
		return (1);
    if (ft_strcmp(cmd->cmd, "echo") == 0) // echo builtin + "-n" option
		return (cmd_echo(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "cd") == 0)
		return (cmd_cd(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "pwd") == 0)
	{
		cmd_pwd(cmd, ctx);
		return(ctx->exit_status);
	}
	else if (ft_strcmp(cmd->cmd, "export") == 0)
		return (cmd_export(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "unset") == 0)
		return (cmd_unset(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "env") == 0)
		return (cmd_env(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "exit") == 0)
		return (cmd_exit(cmd, ctx, shell));
    return (1);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 13:36:54 by airupert          #+#    #+#             */
/*   Updated: 2025/07/01 13:51:09 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// utils used for the builtin functions that will be used for the project

static void	free_str_array(char **arr, int count)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (i < count && arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

// check if string is numeric
int	is_numeric(const char *str)
{
	if (!str || !*str)
		return (0);
	if (*str == '-' || *str == '+')
		str++;
	while (*str)
	{
		if (!(*str >= '0' && *str <= '9'))
			return (0);
		str++;
	}
	return (1);
}

// Update or add an enviornment variable
int	update_env(t_context *ctx, const char *var)
{
	char	*name_end;
	char	*name;
	size_t	name_len;
	int		i;
	int		env_count;
	char	**new_env;

	if (!ctx || !var || !ctx->env)
		return (0);
	name_end = ft_strchr(var, '=');
	if (!name_end || name_end == var)
		return (0);
	name_len = name_end - var;
	name = ft_substr(var, 0, name_len);
	if (!name)
		return (0);
	i = 0;
	while (ctx->env[i])
	{
		if (ft_strncmp(ctx->env[i], name, name_len) == 0 && ctx->env[i][name_len] == '=')
		{
			free(ctx->env[i]);
			ctx->env[i] = ft_strdup(var);
			free(name);
			if (!ctx->env[i])
				return(0);
			return (1);
		}
		i++;
	}
	// add new variable
	env_count = 0;
	while (ctx->env[env_count])
		env_count++;
	new_env = ft_calloc(env_count + 2, sizeof(char *));
	if (!new_env)
	{
		free(name);
		return (0);
	}
	i = 0;
	while (i < env_count)
	{
		new_env[i] = ft_strdup(ctx->env[i]);
		if (!new_env[i])
		{
			free_str_array(new_env, i);
			free(name);
			return (0);
		}
		i++;
	}
	new_env[env_count] = ft_strdup(var);
	if (!new_env[env_count])
	{
		free_str_array(new_env, env_count);
		free(name);
		return (0);
	}
	free_str_array(ctx->env, env_count);
	ctx->env = new_env;
	free(name);
	return (1);
}

// checks for valid enviornment variables
int 	is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

// remove an enviornment variable
int	remove_env(t_context *ctx, const char *name)
{
	if (!name || !*name || !ctx || !ctx->env)
		return (0);
	int	env_count = 0;
	while (ctx->env[env_count])
		env_count++;
	int found = -1;
	int	i = 0;
	while (ctx->env[i])
	{
		if (ft_strncmp(ctx->env[i], name, ft_strlen(name)) == 0 && (ctx->env[i][ft_strlen(name)] == '=' || ctx->env[i][ft_strlen(name)] == '\0'))
		{
			found = i;
			break;
		}
		i++;
	}
	if (found == -1)
		return (1); // variable not found, no error
	free(ctx->env[found]);
	i = found;
	while(i < env_count - 1)
	{
		ctx->env[i] = ctx->env[i + 1];
		i++;
	}
	ctx->env[env_count - 1] = NULL;
	return (1);
}

// helper to retrieve the value of an env variable
char	*get_env_value(char **env, const char *name)
{
	int		i;
	size_t	name_len;

	if (!env || !name)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (env[i] + name_len + 1);
		i++;
	}
	return (NULL);
}/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:33:29 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/22 19:54:10 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Execute a single command
int execute_single_command(Command *cmd, t_context *ctx, t_shell *shell)
{
    int result;
    if (!cmd || !ctx || !shell)
    {
        if (ctx)
            ctx->exit_status = 1;
        return (1);
    }
    if (!cmd->cmd)
	{
		ctx->exit_status = 0;
		return (0);
	}
	printf("DEBUG: execute_single_command: cmd=%s\n", cmd->cmd);
    setup_fds(cmd, ctx); // ensure this is set correctly
	printf("DEBUG: execute_single_command: after setup_fds, in_fd=%d\n", cmd->in_fd);
    if (is_builtin(cmd->cmd))
	{
		printf("DEBUG: execute_single_command: executing builtin\n");
		result = exec_builtin(cmd, ctx, shell);
        ctx->exit_status = result;
		return (result);
	}
    else
	{
		printf("DEBUG: execute_single_command: executing external\n");
		result = exec_external(cmd, ctx);
		ctx->exit_status = result;
		return (result);
	}
}

// Execute command list
int execute_command_list(CommandList *cmd_list, t_context *ctx, t_shell *shell)
{
    CommandList *current;
    int		result;
	int		pipefd[2]; // Array to hold pipe file descriptors
	pid_t	pids[100]; // Array to store PIds (!this can be made dynamic for later if needed!)
	int		prev_fd = STDIN_FILENO; // tracks read end of previous pipe
	int		pid_count = 0; // Counter for PIDS
	char	*path;
	int		i;

	if (!cmd_list || !ctx || !shell)
	{
		if (ctx)
			ctx->exit_status = 1;
		return (1);
	}
    current = cmd_list;
    while (current)
    {
		// Validate command before fork
		if (!is_builtin(current->cmd->cmd))
		{
			path = get_command_path(current->cmd->cmd, ctx);
			if (!path)
			{
				write(STDERR_FILENO, "minishell: command not found: ", 30);
				write(STDERR_FILENO, current->cmd->cmd, ft_strlen(current->cmd->cmd));
				write(STDERR_FILENO, "\n", 1);
				// close previous pipe read end if open
				if (prev_fd != STDIN_FILENO)
					close(prev_fd);
				ctx->exit_status = 127;
				return (127);
			}
			free(path);
		}
		// Check if a pipe is needed (if there is a next command)
		if (current->next)
		{
			if (pipe(pipefd) == -1)
			{
				write(STDERR_FILENO, "minishell: pipe error\n", 22);
				ctx->exit_status = 1;
				return (1);
			}
			// for now just print pipefd values to verify creation (for degbug)
			printf("DEBUG: Pipe created, read fd=%d, write fd=%d\n", pipefd[0], pipefd[1]);
		}
		// fork a child process for the command
		pids[pid_count] = fork();
		if (pids[pid_count] == -1)
		{
			write(STDERR_FILENO, "minishell: fork error\n", 22);
			ctx->exit_status = 1;
			return (1);
		}
		if (pids[pid_count] == 0) // child process
		{
			// redirect input from previous pipe (if not FIRST command)
			if (prev_fd != STDIN_FILENO)
			{
				if (dup2(prev_fd, STDIN_FILENO) == -1)
				{
					write(STDERR_FILENO, "minishell: dup2 error\n", 22);
					exit(1);
				}
				close(prev_fd);
			}
			// redirect output to pipe (if not LAST command)
			if (current->next)
			{
				if (dup2(pipefd[1], STDOUT_FILENO) == -1)
				{
					write(STDERR_FILENO, "minishell: dup2 error\n", 22);
					exit(1);
				}
				close(pipefd[0]);
				close(pipefd[1]);
			}
			// close all other pipe fds
			i = 0;
			while(i < pid_count)
			{
				if (pids[i] != 0)
				{
					if (prev_fd != STDIN_FILENO)
						close(prev_fd);
				}
				i++;
			}
			// setup other fd's (for '<' '>')
			// setup_fds(current->cmd, ctx);
			// execute the command and exit with it's status
			result = execute_single_command(current->cmd, ctx, shell);
			exit(result);
		}
		//Parent process: close used fd's
		if (prev_fd != STDIN_FILENO)
			close(prev_fd);
		if (current->next)
		{
			close(pipefd[1]); //close write end
			prev_fd = pipefd[0]; // save read end for next command
		}
		// Parent process: increment pid_count
		pid_count++;
		current = current->next;
    }
	// close final read end in parent
	if (prev_fd != STDIN_FILENO)
		close(prev_fd);
	// wait for child processes
	i = 0;
	while (i < pid_count)
	{
		int	status;
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status))
			ctx->exit_status = WEXITSTATUS(status);
		i++;
	}
	// temp return (waitpid later)
    return (ctx->exit_status);
}/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:36:08 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/22 19:55:57 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Get path for external command
char *get_command_path(const char *cmd, t_context *ctx)
{
    char	*path;
	char	*paths;
	char	*full_path;
	char	*path_env;
	char	*dir;

	if (!cmd)
	{
		{
			write(2, "minishell: get_command_path: null command\n", 41);
			return (NULL);
		}
	}
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	path_env = get_env_value(ctx->env, "PATH");
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
int exec_external(Command *cmd, t_context *ctx)
{
    char *path;

	if (!cmd || !cmd->cmd || !cmd->args)
	{
		write(STDERR_FILENO, "minishell: exec_external: invalid command\n", 41);
		ctx->exit_status = 1;
		return (1);
	}
	printf("DEBUG: exec_external: cmd=%s, in_fd=%d\n", cmd->cmd, cmd->in_fd);
    path = get_command_path(cmd->cmd, ctx);
    if (!path)
    {
		write(STDERR_FILENO, "minishell: command not found: ", 30);
		write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
		write(STDERR_FILENO, "\n", 1);
        ctx->exit_status = 127;
        return (127);
    }
	printf("DEBUG: exec_external: executing %s\n", path);
	execve(path, cmd->args, ctx->env);
	write(STDERR_FILENO, "minishell: command not found: ", 30);
	write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
	write(STDERR_FILENO, "\n", 1);
    free(path);
    ctx->exit_status = 127;
	return (127);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:05:46 by airupert          #+#    #+#             */
/*   Updated: 2025/07/20 17:57:14 by airupert         ###   ########.fr       */
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:39 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/22 20:03:43 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// handle heredoc redirection
static int	handle_heredoc(Command *cmd, t_context *ctx)
{
	int		pipefd[2];
	char	*line;
	size_t	delim_len;

	if (!cmd->heredoc_delim)
	{
		printf("DEBUG: handle_heredoc: no delimiter\n");
		return (0);
	}

	printf("DEBUG: handle_heredoc: delimiter='%s'\n", cmd->heredoc_delim);
	// create a pipe to store heredoc input
	if (pipe(pipefd) == -1)
	{
		write(STDERR_FILENO, "minishell: warning: heredoc delimited by EOF\n", 45);
		ctx->exit_status = 1;
		return (-1);
	}
	printf("DEBUG: handle_heredoc: pipe created, read fd=%d, write fd=%d\n", pipefd[0], pipefd[1]);
	delim_len = ft_strlen(cmd->heredoc_delim);
	
	// red input until delimiter is found
	while (1)
	{
		printf("DEBUG: handle_heredoc: prompting for input\n");
		line = readline("> ");
		if (!line) // ctrl + D (EOF)
		{
			printf("DEBUG: handle_heredoc: EOF received\n");
			write(STDERR_FILENO, "minishell: warning: heredoc delimited by EOF\n", 45);
			write(STDERR_FILENO, cmd->heredoc_delim, delim_len);
			write(STDERR_FILENO, ")'\n", 3);
			break ;
		}
		printf("DEBUG: handle_heredoc: read line='%s'\n", line);
		if (ft_strncmp(line, cmd->heredoc_delim, delim_len) == 0 && line[delim_len] == '\0')
		{
			printf("DEBUG: handle_heredoc: delimiter matched\n");
			free(line);
			break ;
		}
		if (write(pipefd[1], line, ft_strlen(line)) == -1 || write(pipefd[1], "\n", 1) == -1)
		{
			printf("DEBUG: handle_heredoc: write error\n");
			write(STDERR_FILENO, "minishell: write: cannot write to pipe\n", 38);
			free(line);
			close(pipefd[1]);
			close(pipefd[0]);
			ctx->exit_status = 1;
			return (-1);
		}
		free(line);
	}
	printf("DEBUG: handle_heredoc: closing write end\n");
	// write(pipefd[1], line, ft_strlen(line)); // close write end
	// write(pipefd[1], "\n", 1); // add newline as in bash
	// free(line);
	close(pipefd[1]); // close write end
	cmd->in_fd = pipefd[0]; // set read end for command input
	printf("DEBUG: handle_heredoc: set cmd->in_fd=%d\n", cmd->in_fd);
	ctx->exit_status = 0;
	return (0);
}

// Setup file descriptors for command
void setup_fds(Command *cmd, t_context *ctx)
{
	int		fd;

	printf("DEBUG: setup_fds: in_fd=%d, out_fd=%d, heredoc_delim=%s\n", 
           cmd->in_fd, cmd->out_fd, cmd->heredoc_delim ? cmd->heredoc_delim : "(null)");
    if (cmd->in_fd == -1)
        cmd->in_fd = STDIN_FILENO;
    if (cmd->out_fd == -1)
        cmd->out_fd = STDOUT_FILENO;
    // handle heredoc redirection
    if (cmd->heredoc_delim)
	{
		printf("DEBUG: setup_fds: calling handle_heredoc\n");
		if (handle_heredoc(cmd, ctx) == -1)
			return ;
		printf("DEBUG: setup_fds: heredoc set in_fd=%d\n", cmd->in_fd);
	}
	// handle input redirection
	if (cmd->redirect_in)
	{
		fd = open(cmd->redirect_in, O_RDONLY);
		if (fd == -1)
		{
			write(STDERR_FILENO, "minishell: ", 11);
			write(STDERR_FILENO, cmd->redirect_in, ft_strlen(cmd->redirect_in));
			write(STDERR_FILENO, ": No such file or directory\n", 28);
			ctx->exit_status = 1;
			return ;
		}
		if (cmd->in_fd != STDIN_FILENO)
			close(cmd->in_fd);
		cmd->in_fd = fd;
	}
    // handle output redirection (truncate)
	if (cmd->redirect_out)
	{
		fd = open(cmd->redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		if (fd == -1)
		{
			write(STDERR_FILENO, "minishell: ", 11);
			write(STDERR_FILENO, cmd->redirect_out, ft_strlen(cmd->redirect_out));
			write(STDERR_FILENO, ": Permission denied\n", 20);
			ctx->exit_status = 1;
			return ;
		}
		if (cmd->out_fd != STDOUT_FILENO)
			close(cmd->out_fd);
		cmd->out_fd = fd;
	}
    // handle output redirection (append)
	if (cmd->redirect_append)
	{
		fd = open(cmd->redirect_out, O_WRONLY | O_CREAT | O_APPEND, 0644);
		if (fd == -1)
		{
			write(STDERR_FILENO, "minishell: ", 11);
			write(STDERR_FILENO, cmd->redirect_append, ft_strlen(cmd->redirect_append));
			write(STDERR_FILENO, ": Permission denied\n", 20);
			ctx->exit_status = 1;
			return ;
		}
		if (cmd->out_fd != STDOUT_FILENO)
			close(cmd->out_fd);
		cmd->out_fd = fd;
	}
    // Apply redirection to STDIN and STDOUT
	if (cmd->in_fd != STDIN_FILENO)
	{
		printf("DEBUG: setup_fds: redirecting in_fd=%d to STDIN\n", cmd->in_fd);
		if (dup2(cmd->in_fd, STDIN_FILENO) == -1)
		{
			write(STDERR_FILENO, "minishell: dup2: cannot duplicate file descriptor\n", 50);
			ctx->exit_status = 1;
			return ;
		}
		close(cmd->in_fd);
		cmd->in_fd = STDIN_FILENO;
	}
	if (cmd->out_fd != STDOUT_FILENO)
	{
		if (dup2(cmd->out_fd, STDOUT_FILENO) == -1)
		{
			write(STDERR_FILENO, "minishell: dup2: cannot duplicate file descriptor\n", 50);
			ctx->exit_status = 1;
			return ;
		}
		close(cmd->out_fd);
		cmd->out_fd = STDOUT_FILENO;
	}
}/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:35:30 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/17 20:37:50 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Initialize token list
TokenNode *lexer_init(void)
{
    TokenNode *tokens;
    tokens = malloc(sizeof(TokenNode));
    if (!tokens)
        return (NULL);
    tokens->token.type = TOKEN_END;
    tokens->token.value = NULL;
    tokens->next = NULL;
    return (tokens);
}

// Handle operators (<, >, >>, |)
TokenNode *lexer_operator(char *input, int *i, int *count, TokenNode *tokens)
{
    TokenType type;
    int inc;
    type = TOKEN_END;
    inc = 0;
    lexer_operator_type(input, i, &type, &inc);
    tokens = lexer_operator_value(tokens, *count, type);
    if (!tokens)
        return (NULL);
    *count += 1;
    *i += inc;
    return (tokens);
}

// Handle single-quoted strings
TokenNode *lexer_single_quote(char *input, int *i, int *count, TokenNode *tokens)
{
    int start;
	Token	new_token;
    start = *i;
	(*i)++;
    while (input[*i] && input[*i] != '\'')
        (*i)++;
    if (!input[*i])
    {
        write(2, "minishell: syntax error: unclosed single quote\n", 46);
        free_tokens(tokens);
        return (NULL);
    }
	new_token.type = TOKEN_WORD;
	new_token.value = ft_substr(input, start + 1, *i - start - 1);
	if (!new_token.value)
	{
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		free_tokens(tokens);
		return (NULL);
	}
	printf("DEBUG: lexer_single_quote processing '%.*s'\n", *i - start, input + start);
    tokens = append_token(tokens, new_token);
    if (!tokens)
	{
		free(new_token.value);
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		return (NULL);
	}
	(*count)++;
    (*i)++;
	printf("DEBUG: lexer_single_quote done, i=%d\n", *i);
    return (tokens);
}

// Handle double-quoted strings
TokenNode *lexer_double_quote(char *input, int *i, int *count, TokenNode *tokens)
{
    int start;
	Token	new_token;
    start = *i;
	(*i)++;
    while (input[*i] && input[*i] != '"')
        (*i)++;
    if (!input[*i])
    {
        write(2, "minishell: syntax error: unclosed double quote\n", 46);
        free_tokens(tokens);
        return (NULL);
    }
	new_token.type = TOKEN_WORD;
	new_token.value = ft_substr(input, start + 1, *i - start - 1);
	if (!new_token.value)
	{
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		free_tokens(tokens);
		return (NULL);
	}
	printf("DEBUG: lexer_double_quote processing '%.*s'\n", *i - start, input + start);
    tokens = append_token(tokens, new_token);
    if (!tokens)
	{
		free(new_token.value);
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		return (NULL);
	}
	(*count)++;
    (*i)++;
	printf("DEBUG: lexer_double_quote done, i=%d\n", *i);
    return (tokens);
}

// Main lexer function
TokenNode *lexer(char *input)
{
    int i;
    int count;
    TokenNode	*tokens;
	TokenNode	*current;
    i = 0;
    count = 0;
    tokens = NULL;
    while (input[i])
    {
        while (input[i] == ' ' || input[i] == '\t')
            i++;
        if (!input[i])
            break;
		printf("DEBUG: lexer processing input at i=%d, char='%c'\n", i, input[i]); // issue here
		if ((input[i] == '\'' || input[i] == '"') && tokens && i > 0 && \
		input[i - 1] != ' ' && input[i - 1] != '\t')
		{
			write (2, "minishell: command not found\n", 29);
			free_tokens(tokens);
			return (NULL);
		}
        tokens = lexer_process(input, &i, &count, tokens);
        if (!tokens)
            return (NULL);
		if (!input[i])
			break ;
    }
    if (count == 0)
    {
        tokens = lexer_init();
        return (tokens);
    }
	// append TOKEN_END only if not present
	if (tokens)
	{
		current = tokens;
		while (current->next)
			current = current->next;
		if (current->token.type != TOKEN_END)
			tokens = append_token(tokens, (Token){TOKEN_END, NULL});
	}
	printf("DEBUG: lexer done, count=%d\n", count);
    return (tokens);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operators.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:38:26 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/17 20:42:26 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Determine operator type and increment
void lexer_operator_type(char *input, int *i, TokenType *type, int *inc)
{
    if (input[*i] == '|')
    {
        *type = TOKEN_PIPE;
        *inc = 1; // Advance past the single pipe char
    }
    else if (input[*i] == '<' && input[*i + 1] == '<')
    {
        *type = TOKEN_HEREDOC;
        *inc = 2; // advance past 2 chars
    }
    else if (input[*i] == '>' && input[*i + 1] == '>')
    {
        *type = TOKEN_REDIRECT_APPEND;
        *inc = 2; // advance past 2 chars
    }
    else if (input[*i] == '<')
    {
        *type = TOKEN_REDIRECT_IN;
        *inc = 1;
    }
    else if (input[*i] == '>')
	{
		*type = TOKEN_REDIRECT_OUT;
		*inc = 1;
	}
        
}

// Assign operator token value
TokenNode *lexer_operator_value(TokenNode *tokens, int count, TokenType type)
{
    Token new_token;
    (void)count;
    new_token.type = type;
    new_token.value = NULL;
    tokens = append_token(tokens, new_token);
    if (!tokens)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    return (tokens);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:16:58 by jcouto            #+#    #+#             */
/*   Updated: 2025/05/27 16:06:55 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Free token linked list
void free_tokens(TokenNode *tokens)
{
    TokenNode *tmp;
    while (tokens)
    {
        tmp = tokens->next;
        if (tokens->token.value)
            free(tokens->token.value);
        free(tokens);
        tokens = tmp;
    }
}

// Append a token to the linked list
TokenNode *append_token(TokenNode *tokens, Token token)
{
    TokenNode *new_node;
    TokenNode *current;
    new_node = malloc(sizeof(TokenNode));
    if (!new_node)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        free_tokens(tokens);
        return (NULL);
    }
    new_node->token = token;
    new_node->next = NULL;
    if (tokens && tokens->token.type == TOKEN_END && tokens->next == NULL)
    {
        free(tokens);
        return (new_node);
    }
    if (!tokens)
        return (new_node);
    current = tokens;
    while (current->next)
        current = current->next;
    current->next = new_node;
    return (tokens);
}

// Process tokens for lexer
TokenNode *lexer_process(char *input, int *i, int *count, TokenNode *tokens)
{
    int type;
    type = lexer_classify(input, *i);
    if (type == 0)
        tokens = lexer_word(input, i, count, tokens);
    else if (type == 1)
        tokens = lexer_operator(input, i, count, tokens);
    else if (type == 2)
        tokens = lexer_single_quote(input, i, count, tokens);
    else if (type == 3)
        tokens = lexer_double_quote(input, i, count, tokens);
    else if (type == 4)
        tokens = lexer_variable(input, i, count, tokens);
    return (tokens);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:36:52 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/02 17:07:10 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Handle word tokens
TokenNode *lexer_word(char *input, int *i, int *count, TokenNode *tokens)
{
    int	start;
    int	is_export_arg = 0;
    Token new_token;
    start = *i;
	// Check if arg is after export command
	if (*count > 0 && tokens && tokens->token.type == TOKEN_WORD && \
		ft_strcmp(tokens->token.value, "export")  == 0)
		is_export_arg = 1;
	if (is_export_arg)
	{
		while (input[*i] && (input[*i] != ' ' || (*i > start && input[*i - 1] == '=')) && \
		lexer_classify(input, *i) == 0)
		(*i)++;
	}
	else
	{
		 while (input[*i] && lexer_classify(input, *i) == 0)
        (*i)++;
	}
	if (*i == start)
		return (tokens);
    new_token.type = TOKEN_WORD;
    new_token.value = ft_substr(input, start, *i - start);
    if (!new_token.value)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        free_tokens(tokens);
        return (NULL);
    }
    printf("DEBUG: lexer_word creating token '%s'\n", new_token.value);
    tokens = append_token(tokens, new_token);
    if (!tokens)
    {
        free(new_token.value);
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    (*count)++;
    return (tokens);
}

// Classify character type
int lexer_classify(char *input, int i)
{
    if (input[i] == ' ' || input[i] == '\0')
        return (-1);
    if (input[i] == '|' || input[i] == '<' || input[i] == '>')
        return (1);
    if (input[i] == '\'')
        return (2);
    if (input[i] == '"')
        return (3);
    if (input[i] == '$')
        return (4);
    return (0);
}

// Handle variable tokens
TokenNode *lexer_variable(char *input, int *i, int *count, TokenNode *tokens)
{
    int start;
    Token new_token;
    start = *i;
    (*i)++;
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '?'))
        (*i)++;
    new_token.type = TOKEN_VARIABLE;
    new_token.value = ft_substr(input, start, *i - start);
    if (!new_token.value)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        free_tokens(tokens);
        return (NULL);
    }
    tokens = append_token(tokens, new_token);
    if (!tokens)
    {
        free(new_token.value);
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    (*count)++;
    return (tokens);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 17:28:32 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/21 16:45:38 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Global variables for signal handling and parser
volatile sig_atomic_t g_signal;
TokenNode *g_tokens;
TokenNode *g_current_token;

// Debug: Print token list
static void print_tokens(TokenNode *tokens)
{
    int i;
    i = 0;
    while (tokens)
    {
        const char *type_str;
        switch (tokens->token.type)
        {
            case TOKEN_WORD: type_str = "TOKEN_WORD"; break;
            case TOKEN_VARIABLE: type_str = "TOKEN_VARIABLE"; break;
            case TOKEN_PIPE: type_str = "TOKEN_PIPE"; break;
            case TOKEN_REDIRECT_IN: type_str = "TOKEN_REDIRECT_IN"; break;
            case TOKEN_REDIRECT_OUT: type_str = "TOKEN_REDIRECT_OUT"; break;
            case TOKEN_REDIRECT_APPEND: type_str = "TOKEN_REDIRECT_APPEND"; break;
            case TOKEN_HEREDOC: type_str = "TOKEN_HEREDOC"; break;
            case TOKEN_END: type_str = "TOKEN_END"; break;
            default: type_str = "UNKNOWN"; break;
        }
        printf("  [%d]: type=%s, value=%s\n", i++, type_str, 
               tokens->token.value ? tokens->token.value : "(null)");
        tokens = tokens->next;
    }
}

// Debug: Print command list
static void print_command_list(CommandList *cmd_list)
{
    int cmd_idx;
    cmd_idx = 0;

    while (cmd_list)
    {
        Command *cmd;
        cmd = cmd_list->cmd;
        printf("Command %d:\n", cmd_idx++);
        if (cmd->cmd)
            printf("  cmd: %s (from args[0], TOKEN_WORD)\n", cmd->cmd);
        if (cmd->args && cmd->args[1])
        {
            printf("  args:\n");
            for (int i = 1; cmd->args[i] || i == 1; i++)
            {
                const char *role;
                role = (cmd->args[i] && cmd->args[i][0] == '-') ? "option" : "argument";
                printf("    [%d]: %s (TOKEN_WORD, %s)\n", i - 1, cmd->args[i] ? cmd->args[i] : "(null)", role);
            }
        }
        if (cmd->redirect_in)
            printf("  redirect_in: %s\n", cmd->redirect_in);
        cmd_list = cmd_list->next;
    }
}

// Main helper: Process input
void process_input(char *line, t_shell *shell)
{
	TokenNode	*tokens;
    CommandList	*cmd_list;
    t_context	ctx;
	int			i;
	
	i = 0;
    tokens = lexer(line);
    ctx.pipefd = NULL;
    // ctx.env = shell->env; // removed from original to solve ctx->env update issue
    // ctx.exit_status = 0;
	// copy shell->env to ctx.env
	while (shell->env && shell->env[i])
		i++;
	ctx.env = ft_calloc(i + 1, sizeof(char *));
	if (!ctx.env)
	{
		free_tokens(tokens);
		return ;
	}
	i = 0;
	while (shell->env && shell->env[i])
	{
		ctx.env[i] = ft_strdup(shell->env[i]);
		if (!ctx.env[i])
		{
			while (i > 0)
				free(ctx.env[--i]);
			free(ctx.env);
			free_tokens(tokens);
			return ;
		}
		i++;
	}
	ctx.exit_status = shell->exit_status;
    printf("Tokens:\n");
    print_tokens(tokens);
    cmd_list = parse_program(tokens, &ctx);
    if (cmd_list)
    {
        print_command_list(cmd_list);
        execute_command_list(cmd_list, &ctx, shell);
		shell->exit_status = ctx.exit_status;
		if (shell->env)
		{
			i = 0;
			while (shell->env[i])
				free(shell->env[i++]);
			free(shell->env);
		}
		shell->env = ctx.env;
		ctx.env = NULL; // prevent double-free
        free_command_list(cmd_list);
    }
	else
	{
		// clean up ctx.env if parsing failed
		if (ctx.env)
		{
			i = 0;
			while (ctx.env[i])
				free(ctx.env[i++]);
			free(ctx.env);
		}
		shell->exit_status = 1;
	}
    free_tokens(tokens);
}

// Main helper: Run shell loop
int run_shell_loop(t_shell *shell)
{
    char *line;
    while (1)
    {
        if (shell->exit_flag || g_signal == SIGINT)
        {
			// exit status updated for ctrl+c
			if (g_signal == SIGINT)
				shell->exit_status = 128 + SIGINT;
            g_signal = 0;
            if (shell->exit_flag)
                return (0);
            continue;
        }
        line = readline("minishell> ");
        if (!line)
        {
            printf("exit\n");
            return (0);
        }
        if (*line)
        {
            add_history(line);
            process_input(line, shell);
        }
        free(line);
    }
    return (0);
}

// Main function
int main(int ac, char **av, char **env)
{
    t_shell shell;
    (void)ac;
    (void)av;
    if (init_shell(&shell, env) != 0)
    {
        printf("minishell: initialization failed\n");
        return (EXIT_FAILURE);
    }
    run_shell_loop(&shell);
    cleanup_shell(&shell);
    return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:20:17 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/22 20:17:48 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Parse redirection for command
int parse_redirection(Command *cmd)
{
    TokenType type = TOKEN_END; // initialize to a default

    if (consume(TOKEN_REDIRECT_IN))
        type = TOKEN_REDIRECT_IN;
    else if (consume(TOKEN_REDIRECT_OUT))
        type = TOKEN_REDIRECT_OUT;
    else if (consume(TOKEN_REDIRECT_APPEND))
        type = TOKEN_REDIRECT_APPEND;
    else if (consume(TOKEN_HEREDOC))
        type = TOKEN_HEREDOC;
    
    if (type != TOKEN_END) // check if a redirection was consumed
    {
        printf("DEBUG: parse_redirection: consumed type=%d, next value=%s\n", 
               type, g_current_token ? g_current_token->token.value : "(null)");
        if (!g_current_token || (g_current_token->token.type != TOKEN_WORD && \
            g_current_token->token.type != TOKEN_VARIABLE))
            {
                write(2, "minishell: syntax error near redirection\n", 40);
                return (-1);
            }
        return (parse_redirection_type(cmd, type));
    }
    return (0);
}

// Initialize command structure
Command *parse_command_init(void)
{
    Command *cmd;
    cmd = malloc(sizeof(Command));
    if (!cmd)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    cmd->cmd = NULL;
    cmd->args = NULL;
    cmd->redirect_in = NULL;
    cmd->redirect_out = NULL;
    cmd->redirect_append = NULL;
    cmd->heredoc_delim = NULL;
    cmd->in_fd = STDIN_FILENO;
    cmd->out_fd = STDOUT_FILENO;
    cmd->pid = 0;
    return (cmd);
}

// Process command arguments
Command *parse_command_args(Command *cmd, int *arg_count, t_context *ctx)
{
    cmd->args = parse_args(arg_count, ctx);
    if (!cmd->args)
    {
        free_command(cmd);
        return (NULL);
    }
    if (*arg_count > 0)
        cmd->cmd = ft_strdup(cmd->args[0]);
    if (*arg_count > 0 && !cmd->cmd)
    {
        free_command(cmd);
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    return (cmd);
}

// Parse a single command
Command *parse_command(t_context *ctx)
{
    Command *cmd;
    int arg_count;
    cmd = parse_command_init();
    if (!cmd)
        return (NULL);
    cmd = parse_command_args(cmd, &arg_count, ctx);
    if (!cmd)
        return (NULL);
    while (g_current_token && (g_current_token->token.type == TOKEN_REDIRECT_IN ||
           g_current_token->token.type == TOKEN_REDIRECT_OUT ||
           g_current_token->token.type == TOKEN_REDIRECT_APPEND ||
           g_current_token->token.type == TOKEN_HEREDOC))
    {
        if (parse_redirection(cmd) != 0)
        {
            free_command(cmd);
            return (NULL);
        }
    }
    return (cmd);
}

// Free command structure
void free_command(Command *cmd)
{
    int i;
    if (!cmd)
        return;
    if (cmd->cmd)
        free(cmd->cmd);
    if (cmd->args)
    {
        i = 0;
        while (cmd->args[i])
            free(cmd->args[i++]);
        free(cmd->args);
    }
    if (cmd->redirect_in)
        free(cmd->redirect_in);
    if (cmd->redirect_out)
        free(cmd->redirect_out);
    if (cmd->redirect_append)
        free(cmd->redirect_append);
    if (cmd->heredoc_delim)
        free(cmd->heredoc_delim);
    free(cmd);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:31:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/20 15:31:13 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Initialize command list
CommandList *init_command_list(t_context *ctx)
{
    CommandList *list;
    list = malloc(sizeof(CommandList));
    if (!list)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    list->cmd = parse_command(ctx);
    list->next = NULL;
    if (!list->cmd)
    {
        free(list);
        return (NULL);
    }
    return (list);
}

// Handle pipe in command list
// changing arguments for function
CommandList *handle_pipe(CommandList *list, t_context *ctx)
{
    if (consume(TOKEN_PIPE))
    {
        list->next = parse_program(g_current_token, ctx); // Use g_current_token
        if (!list->next)
        {
            write(2, "minishell: syntax error near '|'\n", 32);
            free_command_list(list);
            return (NULL);
        }
    }
    return (list);
}

// Parse program: command_list
CommandList *parse_program(TokenNode *token_stream, t_context *ctx)
{
    CommandList *list;
    g_tokens = token_stream;
    g_current_token = token_stream;
    list = init_command_list(ctx);
	char	*path;
    if (!list)
        return (NULL);
    // validate command checker (check for non-empty cmd)
    if (list->cmd->cmd && !is_builtin(list->cmd->cmd))
    {
		path = get_command_path(list->cmd->cmd, ctx);
		if (!path)
		{
			write(2, "minishell: command not found: ", 30);
			write(2, list->cmd->cmd, ft_strlen(list->cmd->cmd));
			write(2, "\n", 1);
			free_command_list(list);
			return (NULL);
		}
		free(path);
    }
    list = handle_pipe(list, ctx); // removed token stream
    if (!list)
        return (NULL);
    if (g_current_token && g_current_token->token.type != TOKEN_END)
    {
        write(2, "minishell: syntax error: unexpected tokens\n", 42);
        free_command_list(list);
        return (NULL);
    }
    return (list);
}

// Parse command_list: ( command ( PIPE command )* )
CommandList *parse_command_list(t_context *ctx)
{
    CommandList *list;
    list = init_command_list(ctx);
    if (!list)
        return (NULL);
    if (consume(TOKEN_PIPE))
    {
        list->next = parse_command_list(ctx);
        if (!list->next)
        {
            write(2, "minishell: syntax error near '|'\n", 32);
            free_command_list(list);
            return (NULL);
        }
    }
    return (list);
}

// Free command list
void free_command_list(CommandList *list)
{
    CommandList *tmp;
    while (list)
    {
        tmp = list->next;
        if (list->cmd)
            free_command(list->cmd);
        free(list);
        list = tmp;
    }
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:19:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/22 20:02:02 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Consume a token of expected type
int consume(TokenType type)
{

    if (g_current_token && g_current_token->token.type == type)
    {
		printf("DEBUG: consume: consumed type=%d\n", type);
        g_current_token = g_current_token->next;
        return (1);
    }
    return (0);
}

// Allocate memory for args array
char **parse_args_alloc(int *arg_count)
{
    TokenNode   *temp;
	// char		*combined;
	int			len;

    temp = g_current_token;
	*arg_count = 0;
    while (temp && (temp->token.type == TOKEN_WORD || temp->token.type == TOKEN_VARIABLE))
    {
		if (temp->token.type == TOKEN_WORD && temp->token.value && temp->token.value[0] == '/')
		{
			len = 0;
			// combined = NULL;
			while (temp && temp->token.type == TOKEN_WORD && temp->token.value && temp->token.value[0] != ' ')
			{
				len += ft_strlen(temp->token.value);
				temp = temp->next;
			}
			(*arg_count)++;
		}
		else
		{
			(*arg_count)++;
			temp = temp->next;
		} 
    }
    return (malloc((*arg_count + 1) * sizeof(char *)));
}

// Process arguments into array
char **parse_args_process(char **args, int *arg_count, t_context *ctx)
{
    int		i;
    char	*value;
	char	*combined;
	int		len;
	char	*tmp;

    i = 0;
    while (g_current_token && (g_current_token->token.type == TOKEN_WORD || 
           g_current_token->token.type == TOKEN_VARIABLE))
    {
		// the parser expansion and modification for echo $?
		if (g_current_token->token.type == TOKEN_VARIABLE && g_current_token->token.value &&
			g_current_token->token.value[0] == '/')
		{
			len = 0;
			combined = NULL;
			while(g_current_token && g_current_token->token.type == TOKEN_WORD &&
				g_current_token->token.value && g_current_token->token.value[0] != ' ')
			{
				len += ft_strlen(g_current_token->token.value);
				if (!combined)
					combined = ft_strdup(g_current_token->token.value);
				else
				{
					tmp = ft_strjoin(combined, g_current_token->token.value);
					free(combined);
					combined = tmp;
				}
				g_current_token = g_current_token->next;
			}
			args[i] = combined;
			if (!args[i])
			{
				while(i > 0)
					free(args[--i]);
				free(args);
				return (NULL);
			}
			i++;
		}
		else
		{
			if (g_current_token->token.type == TOKEN_VARIABLE)
			{
				if (ft_strcmp(g_current_token->token.value, "$?" ) == 0)
				{
					args[i] = ft_itoa(ctx->exit_status);
					if (!args[i])
					{
						while(i > 0)
							free(args[--i]);
						free(args);
						write(2, "minishell: malloc: cannot allocate memory\n", 41);
						return (NULL);
					}
					printf("DEBUG: Expanded $? to '%s'\n", args[i]); // debugger
					g_current_token = g_current_token->next;
				}
				else
				{
					value = get_env_value(ctx->env, g_current_token->token.value + 1);
					args[i] = ft_strdup(value ? value : "");
					if (!args[i])
					{
						while (i > 0)
							free(args);
						write(2, "minishell: malloc: cannot allocate memory\n", 41);
						return (NULL);
					}
					printf("DEBUG: Expanded variable '%s' to '%s'\n", g_current_token->token.value, args[i]);
					g_current_token = g_current_token->next;
				}
			}
			else
			{
				if (!g_current_token->token.value)
				{
					write(2, "minishell: parse_args_process: null token value\n", 47);
					while (i > 0)
						free(args[--i]);
					return (free(args), NULL);
				}
				args[i] = ft_strdup(g_current_token->token.value);
				if (!args[i])
				{
					while(i > 0)
						free(args[--i]);
					return (free(args), NULL);
				}
				g_current_token = g_current_token->next;
			}
			i++;
		}
	}
	args[i] = NULL;
	*arg_count = i;
	return (args);
}

// Parse arguments
char **parse_args(int *arg_count, t_context *ctx)
{
    char **args;
    *arg_count = 0;
    args = parse_args_alloc(arg_count);
    if (!args)
        return (NULL);
    args = parse_args_process(args, arg_count, ctx);
    return (args);
}

// Handle redirection type
int parse_redirection_type(Command *cmd, TokenType type)
{
    char *value;
    if (!g_current_token || (g_current_token->token.type != TOKEN_WORD && 
        g_current_token->token.type != TOKEN_VARIABLE))
        return (-1);
    value = ft_strdup(g_current_token->token.value);
    if (!value)
        return (-1);
    g_current_token = g_current_token->next;
    if (type == TOKEN_REDIRECT_IN)
        cmd->redirect_in = value;
    else if (type == TOKEN_REDIRECT_OUT)
        cmd->redirect_out = value;
    else if (type == TOKEN_REDIRECT_APPEND)
        cmd->redirect_append = value;
    else if (type == TOKEN_HEREDOC)
        cmd->heredoc_delim = value;
    return (0);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:25:06 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/02 14:53:20 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int sig)
{
    g_signal = sig;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
// Signal handler for SIGQUIT (Ctrl+\)
void handle_sigquit(int sig)
{
    g_signal = sig;
    (void)sig;
}
// Initialize shell environment
int init_shell(t_shell *shell, char **envp)
{
    int i;

    shell->exit_flag = 0;
    shell->exit_status = 0;
    i = 0;
    while (envp[i])
        i++;
    shell->env = ft_calloc(i + 1, sizeof(char *));
    if (!shell->env)
        return (-1);
//    i = 0;
    if (copy_env(shell, envp, i) != 0)
        return (-1);
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    return (0);
}
//Copy environment variables
int copy_env(t_shell *shell, char **envp, int env_count)
{
    int i = 0;
    while (i < env_count)
    {
        shell->env[i] = ft_strdup(envp[i]);
        if (!shell->env[i])
        {
            printf("minishell: strdup: cannot allocate memory\n");
            while (i > 0)
                free(shell->env[--i]);
            free(shell->env);
            return (-1);
        }
        i++;
    }
    shell->env[i] = NULL;
    return (0);
}
// Free shell resources
void cleanup_shell(t_shell *shell)
{
    int i;

    if (shell && shell->env)
    {
        i = 0;
        while (shell->env[i])
        {
            free(shell->env[i]);
            i++;
        }
        free(shell->env);
        shell->env = NULL;
    }
}
