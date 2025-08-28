/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:33:29 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:28:25 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Execute a single command
int execute_single_command(Command *cmd, t_shell *shell)
{
	int result;
	if (!cmd || !shell || !cmd->cmd)
	{
		shell->exit_status = 1;
		return (1);
	}
	setup_fds(cmd, shell);
	if (is_builtin(cmd->cmd))
	{
		result = exec_builtin(cmd, shell);
		shell->exit_status = result;
		return (result);
	}
	result = execute_external(cmd, shell);
	shell->exit_status = result;
	return (result);
}

int execute_command_list(CommandList *cmd_list, t_shell *shell)
{
	int stdin_save;
	int stdout_save;
	int result;

	if (!cmd_list || !shell)
	{
		shell->exit_status = 1;
		return 1;
	}
	stdin_save = dup(STDIN_FILENO);
	stdout_save = dup(STDOUT_FILENO);
	if (stdin_save == -1 || stdout_save == -1)
	{
		write(STDERR_FILENO, "minishell: dup error\n", 21);
		shell->exit_status = 1;
		if (stdin_save != -1)
			close(stdin_save);
		if (stdout_save != -1)
			close(stdout_save);
		return 1;
	}

	if (!cmd_list->next)
		result = execute_single_command(cmd_list->cmd, shell);
	else
		result = execute_pipeline(cmd_list, shell);
	if (dup2(stdin_save, STDIN_FILENO) == -1)
	{
		write(STDERR_FILENO, "minishell: dup2 restore error\n", 29);
		shell->exit_status = 1;
	}
	if (dup2(stdout_save, STDOUT_FILENO) == -1)
	{
		write(STDERR_FILENO, "minishell: dup2 restore error\n", 29);
		shell->exit_status = 1;
	}

	close(stdin_save);
	close(stdout_save);
	return result;
}

int execute_pipeline(CommandList *cmd_list, t_shell *shell)
{
	pid_t pids[100];
	int pid_count;
	int prev_fd;
	CommandList *current;
	int status;
	int i;
	int result;

	pid_count = 0;
	prev_fd = STDIN_FILENO;
	current = cmd_list;
	result = 0;

	while (current)
	{
		int pipefd[2];

		if (current->next)
		{
			if (pipe(pipefd) == -1)
			{
				write(STDERR_FILENO, "minishell: pipe error\n", 22);
				shell->exit_status = 1;
				return 1;
			}
		}
		pids[pid_count] = fork();
		if (pids[pid_count] == -1)
		{
			write(STDERR_FILENO, "minishell: fork error\n", 22);
			shell->exit_status = 1;
			if (current->next)
			{
				close(pipefd[0]);
				close(pipefd[1]);
			}
			return 1;
		}

		if (pids[pid_count] == 0)
		{
			printf("DEBUG: Child for cmd=%s, initial STDIN=%d, STDOUT=%d\n",
				   current->cmd->cmd, STDIN_FILENO, STDOUT_FILENO);
			signal(SIGINT, SIG_DFL); // reset SIGINT
			signal(SIGQUIT, SIG_DFL); // reset SIGQUIT
			if (prev_fd != STDIN_FILENO)
			{
				if (dup2(prev_fd, STDIN_FILENO) == -1)
				{
					write(STDERR_FILENO, "minishell: dup2 error\n", 22);
					exit(1);
				}
				close(prev_fd);
				printf("DEBUG: Child for cmd=%s, STDIN redirected to %d\n",
					   current->cmd->cmd, STDIN_FILENO);
			}
			if (current->next)
			{
				if (dup2(pipefd[1], STDOUT_FILENO) == -1)
				{
					write(STDERR_FILENO, "minishell: dup2 error\n", 22);
					exit(1);
				}
				close(pipefd[0]);
				close(pipefd[1]);
				printf("DEBUG: Child for cmd=%s, STDOUT redirected to %d\n",
					   current->cmd->cmd, STDOUT_FILENO);
			}
			printf("DEBUG: Child for cmd=%s, calling setup_fds\n", current->cmd->cmd);
			setup_fds(current->cmd, shell);
			printf("DEBUG: Child for cmd=%s, after setup_fds, STDIN=%d, STDOUT=%d\n",
				   current->cmd->cmd, STDIN_FILENO, STDOUT_FILENO);
			result = execute_single_command(current->cmd, shell);
			exit(result);
		}

		if (prev_fd != STDIN_FILENO)
			close(prev_fd);
		if (current->next)
		{
			close(pipefd[1]);
			prev_fd = pipefd[0];
			printf("DEBUG: Parent, closed write_fd=%d, prev_fd=%d\n", pipefd[1], prev_fd);
		}
		pid_count++;
		current = current->next;
	}

	if (prev_fd != STDIN_FILENO)
		close(prev_fd);

	i = 0;
	while (i < pid_count)
	{
		waitpid(pids[i], &status, 0);
		if (i == pid_count - 1)
		{
			if (WIFEXITED(status))
				shell->exit_status = WEXITSTATUS(status);
			else
				shell->exit_status = 1;
		}
		i++;
	}

	return shell->exit_status;
}
