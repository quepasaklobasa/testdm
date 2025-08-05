/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:33:29 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/31 20:43:12 by jcouto           ###   ########.fr       */
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
    if (is_builtin(cmd->cmd))
	{
		result = exec_builtin(cmd, ctx, shell);
        ctx->exit_status = result;
		return (result);
	}
    else
	{
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
		// Remove command validation from here - let the child process handle it
		// This allows the pipeline to be set up properly even if a command fails
		
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
			printf("DEBUG: Child process: setting up fds for cmd=%s\n", current->cmd->cmd);
			setup_fds(current->cmd, ctx); //setup_fds is now a child process
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
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:36:08 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/22 20:27:13 by airupert         ###   ########.fr       */
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
    path = get_command_path(cmd->cmd, ctx);
    if (!path)
    {
		write(STDERR_FILENO, "minishell: command not found: ", 30);
		write(STDERR_FILENO, cmd->cmd, ft_strlen(cmd->cmd));
		write(STDERR_FILENO, "\n", 1);
        ctx->exit_status = 127;
        return (127);
    }
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
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:39 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/24 20:34:59 by jcouto           ###   ########.fr       */
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
		return (0);

	// create a pipe to store heredoc input
	if (pipe(pipefd) == -1)
	{
		write(STDERR_FILENO, "minishell: warning: heredoc delimited by EOF\n", 45);
		ctx->exit_status = 1;
		return (-1);
	}
	delim_len = ft_strlen(cmd->heredoc_delim);
	
	// red input until delimiter is found
	while (1)
	{
		line = readline("> ");
		// rl_on_new_line(); // may not need?
		// rl_redisplay(); // also may not need
		if (!line) // ctrl + D (EOF)
		{
			write(STDERR_FILENO, "minishell: warning: heredoc delimited by EOF\n", 45);
			write(STDERR_FILENO, cmd->heredoc_delim, delim_len);
			write(STDERR_FILENO, ")'\n", 3);
			break ;
		}
		if (g_signal == SIGINT) // Crtl+C
		{
			free(line);
			close(pipefd[1]);
			close(pipefd[0]);
			ctx->exit_status = 128 + SIGINT;
			g_signal = 0;
			return (-1);
		}
		if (ft_strncmp(line, cmd->heredoc_delim, delim_len) == 0 && line[delim_len] == '\0')
		{
			free(line);
			break ;
		}
		if (write(pipefd[1], line, ft_strlen(line)) == -1 || write(pipefd[1], "\n", 1) == -1)
		{
			write(STDERR_FILENO, "minishell: write: cannot write to pipe\n", 38);
			free(line);
			close(pipefd[1]);
			close(pipefd[0]);
			ctx->exit_status = 1;
			return (-1);
		}
		free(line);
	}
	// write(pipefd[1], line, ft_strlen(line)); // close write end
	// write(pipefd[1], "\n", 1); // add newline as in bash
	// free(line);
	close(pipefd[1]); // close write end
	cmd->in_fd = pipefd[0]; // set read end for command input
	ctx->exit_status = 0;
	return (0);
}

// Setup file descriptors for command
void setup_fds(Command *cmd, t_context *ctx)
{
	int		fd;

    if (cmd->in_fd == -1)
        cmd->in_fd = STDIN_FILENO;
    if (cmd->out_fd == -1)
        cmd->out_fd = STDOUT_FILENO;
    // handle heredoc redirection
    if (cmd->heredoc_delim)
	{
		if (handle_heredoc(cmd, ctx) == -1)
			return ;
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
		fd = open(cmd->redirect_append, O_WRONLY | O_CREAT | O_APPEND, 0644);
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
}