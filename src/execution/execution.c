/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:33:29 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/12 10:05:52 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Execute a single command
int execute_single_command(Command *cmd, t_shell *shell)
{
    int result;
    if (!cmd || !shell)
    {
        shell->exit_status = 1;
        return (1);
    }
    if (!cmd->cmd)
	{
		shell->exit_status = 0;
		return (0);
	}
    if (is_builtin(cmd->cmd))
	{
		result = exec_builtin(cmd, shell);
        shell->exit_status = result;
		return (result);
	}
	result = exec_external(cmd, shell);
	shell->exit_status = result;
	return (result);
}

// Execute command list
int execute_command_list(CommandList *cmd_list, t_shell *shell)
{
    CommandList *current;
    int		result;
	pid_t	pids[100]; // Array to store PIds (!this can be made dynamic for later if needed!)
	int		pid_count; // Counter for PIDS
	int		i;
	int		stdout_save;

	if (!cmd_list || !shell)
	{
		shell->exit_status = 1;
		return (1);
	}
    current = cmd_list;
	pid_count = 0;
	stdout_save = -1; 
	shell->prev_fd = STDIN_FILENO; // Initialize previous fd to stdin
    while (current)
    {
		if (current->next)
		{
			if (pipe(shell->pipefd) == -1)
			{
				write(STDERR_FILENO, "minishell: pipe error\n", 22);
				shell->exit_status = 1;
				return (1);
			}
			// for now just print pipefd values to verify creation (for debug)
            current->cmd->out_fd = shell->pipefd[1];
			printf("DEBUG: Pipe created, read fd=%d, write fd=%d\n", shell->pipefd[0], shell->pipefd[1]);
			stdout_save = dup(STDOUT_FILENO); // Save stdout for later restoration
			if (stdout_save == -1)
			{
				write(STDERR_FILENO, "minishell: fork error\n", 22);
				shell->exit_status = 1;
				return (1);
			}
			if (dup2(shell->pipefd[1], STDIN_FILENO) == -1) // child process
			{
			// redirect input from previous pipe (if not FIRST command)
				write(STDERR_FILENO, "minishell: dup2 error\n", 22);
				shell->exit_status = 1;
				close(shell->pipefd[0]);
				close(shell->pipefd[1]);
				close(stdout_save);
				return (1);
			}
		}
			// redirect output to pipe (if not LAST command)
			if (is_builtin(current->cmd->cmd) && (!current->next || pid_count == 0))
      		{
            	printf("DEBUG: Running built-in %s in parent\n", current->cmd->cmd);
            	setup_fds(current->cmd, shell);
            	result = execute_single_command(current->cmd, shell);
            	if (current->next)
            	{
                	if (dup2(stdout_save, STDOUT_FILENO) == -1)
                	{
                 	   write(STDERR_FILENO, "minishell: dup2 error\n", 22);
                 	   shell->exit_status = 1;
                  	  close(shell->pipefd[0]);
                  	  close(shell->pipefd[1]);
                    	close(stdout_save);
                    	return (1);
					}	  
					close(stdout_save);
                   	close(shell->pipefd[1]);
                   shell->prev_fd = shell->pipefd[0];
               }
           }
       
       else
        {
            pids[pid_count] = fork();
            if (pids[pid_count] == -1)
            {
                write(STDERR_FILENO, "minishell: fork error\n", 22);
                shell->exit_status = 1;
                if (current->next)
                {
                    close(shell->pipefd[0]);
                    close(shell->pipefd[1]);
                }
                if (stdout_save != -1)
                    close(stdout_save);
                return (1);
            }
            if (pids[pid_count] == 0)
            {
                if (shell->prev_fd != STDIN_FILENO)
                {
                    if (dup2(shell->prev_fd, STDIN_FILENO) == -1)
                    {
                        write(STDERR_FILENO, "minishell: dup2 error\n", 22);
                        exit(1);
                    }
                    close(shell->prev_fd);
                }
                if (current->next)
                {
                    if (dup2(shell->pipefd[1], STDOUT_FILENO) == -1)
                    {
                        write(STDERR_FILENO, "minishell: dup2 error\n", 22);
                        exit(1);
                    }
                    close(shell->pipefd[0]);
                    close(shell->pipefd[1]);
                }
                printf("DEBUG: Child process: setting up fds for cmd=%s\n", current->cmd->cmd);
                setup_fds(current->cmd, shell);
                result = execute_single_command(current->cmd, shell);
                exit(result);
            }
            if (current->next)
            {
                close(shell->pipefd[1]);
                shell->prev_fd = shell->pipefd[0];
            }
            pid_count++;
        }
        if (shell->prev_fd != STDIN_FILENO && !current->next)
            close(shell->prev_fd);
        current = current->next;
    }
    if (stdout_save != -1)
        close(stdout_save);
    i = 0;
    while (i < pid_count)
    {
        int status;
        waitpid(pids[i], &status, 0);
        if (WIFEXITED(status) && i == pid_count - 1)
            shell->exit_status = WEXITSTATUS(status);
        i++;
    }
    return (shell->exit_status);
}
