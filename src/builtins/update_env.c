/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   update_env.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 21:21:53 by airupert          #+#    #+#             */
/*   Updated: 2025/08/28 21:41:14 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char	*extract_var_name(const char *var, size_t *name_len)
{
	char	*name_end;
	char	*name;

	name_end = ft_strchr(var, '=');
	if (!name_end || name_end == var)
		return (NULL);
	*name_len = name_end - var;
	name = ft_substr(var, 0, *name_len);
	if (!name)
		return (NULL);
	return (name);
}

static int	find_env_var(t_shell *shell, const char *name, size_t name_len)
{
	int	i;

	i = 0;
	while (i < shell->env_count)
	{
		if (ft_strncmp(shell->env[i], name, name_len) == 0 \
			&& shell->env[i][name_len] == '=')
			return (1);
		i++;
	}
	return (-1);
}

static int	update_existing_var(t_shell *shell, int index, const char *var)
{
	free(shell->env[index]);
	shell->env[index] = ft_strdup(var);
	return (shell->env[index] != NULL);
}

static int	add_new_var(t_shell *shell, const char *var)
{
	if (shell->env_count >= shell->env_capacity - 1)
	{
		write(STDERR_FILENO, "minishell: env: \
			too many enviornment variables\n", 47);
		return (0);
	}
	shell->env[shell->env_count] = ft_strdup(var);
	if (!shell->env[shell->env_count])
		return (0);
	shell->env[++shell->env_count] = NULL;
	return (1);
}

// Update or add an enviornment variable
int	update_env(t_shell *shell, const char *var)
{
	char	*name;
	size_t	name_len;
	int		index;

	if (!shell || !var || !shell->env)
		return (0);
	name = extract_var_name(var, &name_len);
	if (!name)
		return (0);
	index = find_env_var(shell, name, name_len);
	if (index >= 0)
	{
		free(name);
		return (update_existing_var(shell, index, var));
	}
	free(name);
	return (add_new_var(shell, var));
}
