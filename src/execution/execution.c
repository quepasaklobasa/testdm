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
