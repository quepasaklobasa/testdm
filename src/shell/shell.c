/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:25:06 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:29:37 by jcouto           ###   ########.fr       */
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
}
// Initialize shell environment
int init_shell(t_shell *shell, char **envp)
{
	int i;

	shell->pid = getpid();
	shell->exit_flag = 0;
	shell->exit_status = 0;
	shell->pipefd[0] = -1;
	shell->pipefd[1] = -1;
	shell->prev_fd = STDIN_FILENO; 
	shell->env_count = 0;
	shell->env_capacity = 200;
	i = 0;
	while (envp[i])
		i++;
	shell->env = malloc(sizeof(char*) * shell->env_capacity);
	if (!shell->env)
		return (-1);
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
	while (i < env_count && i < shell->env_capacity - 1)
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
	shell->env_count = i;
	return (0);
}
// Free shell resources
void cleanup_shell(t_shell *shell)
{
	int i;

	if (shell && shell->env)
	{
		i = 0;
		while (i < shell->env_count)
		{
			free(shell->env[i]);
			i++;
		}
		free(shell->env);
		shell->env = NULL;
		shell->env_count = 0;
	}
}
