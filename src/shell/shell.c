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
