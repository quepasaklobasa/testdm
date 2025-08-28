/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 13:36:54 by airupert          #+#    #+#             */
/*   Updated: 2025/08/28 22:02:27 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

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

// checks for valid enviornment variables
int	is_valid_identifier(const char *str)
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

static int	find_and_remove_env(t_shell *shell, \
	const char *name, size_t name_len)
{
	int		i;
	int		found;

	i = 0;
	found = -1;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], name, name_len) == 0 && \
		(shell->env[i][name_len] == '=' || shell->env[i][name_len] == '\0'))
		{
			found = 1;
			break ;
		}
		i++;
	}
	return (found);
}

int	remove_env(t_shell *shell, const char *name)
{
	size_t	name_len;
	int		found;

	if (!name || !*name || !shell || !shell->env)
		return (1);
	name_len = ft_strlen(name);
	found = find_and_remove_env(shell, name, name_len);
	if (found == -1)
		return (0);
	free(shell->env[found]);
	while (shell->env[found + 1])
	{
		shell->env[found] = shell->env[found + 1];
		found++;
	}
	shell->env[found] = NULL;
	shell->env_count--;
	return (0);
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
		if (env[i][0] == '\0')
		{
			i++;
			continue ;
		}
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (env[i] + name_len + 1);
		i++;
	}
	return (NULL);
}
