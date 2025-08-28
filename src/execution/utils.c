/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:39 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/26 16:44:57 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// handle heredoc redirection
static int	handle_heredoc(Command *cmd, t_shell *shell)
{
	int		pipefd[2];
	char	*line;
	size_t	delim_len;
	char	*expanded_line;

	if (!cmd->heredoc_delim)
		return (0);

	if (pipe(pipefd) == -1)
	{
		write(STDERR_FILENO, "minishell: pipe error\n", 22);
		shell->exit_status = 1;
		return (1);
	}
	delim_len = ft_strlen(cmd->heredoc_delim);
	while (1)
	{
		line = readline("> ");
		if (!line)
		{
			write(STDERR_FILENO, "minishell: warning: heredoc delimited by EOF\n", 45);
			write(STDERR_FILENO, cmd->heredoc_delim, delim_len);
			write(STDERR_FILENO, ")'\n", 3);
			close(pipefd[1]);
			cmd->in_fd = pipefd[0];
			shell->exit_status = 0;
			return (0);
		}
		if (g_signal == SIGINT) // Crtl+C
		{
			free(line);
			close(pipefd[1]);
			close(pipefd[0]);
			shell->exit_status = 128 + SIGINT;
			g_signal = 0;
			return (1);
		}

		if (ft_strncmp(line, cmd->heredoc_delim, delim_len) == 0 && line[delim_len] == '\0')
		{
			free(line);
			break ;
		}

		if (!cmd->heredoc_quoted)
			expanded_line = expand_variables_in_string(line, shell);
		else
			expanded_line = ft_strdup(line);
		free(line);
		if (!expanded_line)
		{
			write(STDERR_FILENO, "minishell: malloc error\n", 24);
			close(pipefd[1]);
			close(pipefd[0]);
			shell->exit_status = 1;
			return (1);
		}
		if (write(pipefd[1], expanded_line, ft_strlen(expanded_line)) == -1 || write(pipefd[1], "\n", 1) == -1)
		{
			write(STDERR_FILENO, "minishell: write: cannot write to pipe\n", 38);
			free(expanded_line);
			close(pipefd[1]);
			close(pipefd[0]);
			shell->exit_status = 1;
			return (1);
		}
		free(expanded_line);
	}
	close(pipefd[1]);
	cmd->in_fd = pipefd[0];
	shell->exit_status = 0;
	return (0);
}

// Setup file descriptors for command
void setup_fds(Command *cmd, t_shell *shell)
{
    int fd;

    if (cmd->in_fd == -1)
        cmd->in_fd = STDIN_FILENO;
    if (cmd->out_fd == -1)
        cmd->out_fd = STDOUT_FILENO;
    
    if (cmd->heredoc_delim)
    {
        if (handle_heredoc(cmd, shell) == -1)
            return;
    }
    
    if (cmd->redirect_in)
    {
        fd = open(cmd->redirect_in, O_RDONLY);
        if (fd == -1)
        {
            write(STDERR_FILENO, "minishell: ", 11);
            write(STDERR_FILENO, cmd->redirect_in, ft_strlen(cmd->redirect_in));
            write(STDERR_FILENO, ": No such file or directory\n", 28);
            shell->exit_status = 1;
            return;
        }
        if (cmd->in_fd != STDIN_FILENO)
            close(cmd->in_fd);
        cmd->in_fd = fd;
    }
    
    if (cmd->redirect_out)
    {
        fd = open(cmd->redirect_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            write(STDERR_FILENO, "minishell: ", 11);
            write(STDERR_FILENO, cmd->redirect_out, ft_strlen(cmd->redirect_out));
            write(STDERR_FILENO, ": Permission denied\n", 20);
            shell->exit_status = 1;
            return;
        }
        if (cmd->out_fd != STDOUT_FILENO)
            close(cmd->out_fd);
        cmd->out_fd = fd;
    }
    
    if (cmd->redirect_append)
    {
        fd = open(cmd->redirect_append, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd == -1)
        {
            write(STDERR_FILENO, "minishell: ", 11);
            write(STDERR_FILENO, cmd->redirect_append, ft_strlen(cmd->redirect_append));
            write(STDERR_FILENO, ": Permission denied\n", 20);
            shell->exit_status = 1;
            return;
        }
        if (cmd->out_fd != STDOUT_FILENO)
            close(cmd->out_fd);
        cmd->out_fd = fd;
    }
    
    if (cmd->in_fd != STDIN_FILENO)
    {
        if (dup2(cmd->in_fd, STDIN_FILENO) == -1)
        {
            write(STDERR_FILENO, "minishell: dup2: cannot duplicate file descriptor\n", 50);
            shell->exit_status = 1;
            return;
        }
        close(cmd->in_fd);
        cmd->in_fd = STDIN_FILENO;
    }
    
    if (cmd->out_fd != STDOUT_FILENO)
    {
        if (dup2(cmd->out_fd, STDOUT_FILENO) == -1)
        {
            write(STDERR_FILENO, "minishell: dup2: cannot duplicate file descriptor\n", 50);
            shell->exit_status = 1;
            return;
        }
        close(cmd->out_fd);
        cmd->out_fd = STDOUT_FILENO;
    }
}