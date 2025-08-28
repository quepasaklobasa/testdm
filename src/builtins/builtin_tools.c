/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_tools.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 15:42:06 by airupert          #+#    #+#             */
/*   Updated: 2025/08/28 22:03:23 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

void	write_error(char *msg, int status, t_shell *shell)
{
	int	len;

	len = ft_strlen(msg);
	write(STDERR_FILENO, msg, len);
	shell->exit_status = status;
}

char	**copy_env_array(t_shell *shell)
{
	int		i;
	char	**sorted_env;

	sorted_env = malloc(sizeof(char *) * (shell->env_count + 1));
	if (!sorted_env)
		return (NULL);
	i = 0;
	while (i < shell->env_count)
	{
		sorted_env[i] = shell->env[i];
		i++;
	}
	sorted_env[i] = NULL;
	return (sorted_env);
}
