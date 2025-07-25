/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 13:36:54 by airupert          #+#    #+#             */
/*   Updated: 2025/07/01 13:51:09 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// utils used for the builtin functions that will be used for the project

static void	free_str_array(char **arr, int count)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (i < count && arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
}

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

// Update or add an enviornment variable
int	update_env(t_context *ctx, const char *var)
{
	char	*name_end;
	char	*name;
	size_t	name_len;
	int		i;
	int		env_count;
	char	**new_env;

	if (!ctx || !var || !ctx->env)
		return (0);
	name_end = ft_strchr(var, '=');
	if (!name_end || name_end == var)
		return (0);
	name_len = name_end - var;
	name = ft_substr(var, 0, name_len);
	if (!name)
		return (0);
	i = 0;
	while (ctx->env[i])
	{
		if (ft_strncmp(ctx->env[i], name, name_len) == 0 && ctx->env[i][name_len] == '=')
		{
			free(ctx->env[i]);
			ctx->env[i] = ft_strdup(var);
			free(name);
			if (!ctx->env[i])
				return(0);
			return (1);
		}
		i++;
	}
	// add new variable
	env_count = 0;
	while (ctx->env[env_count])
		env_count++;
	new_env = ft_calloc(env_count + 2, sizeof(char *));
	if (!new_env)
	{
		free(name);
		return (0);
	}
	i = 0;
	while (i < env_count)
	{
		new_env[i] = ft_strdup(ctx->env[i]);
		if (!new_env[i])
		{
			free_str_array(new_env, i);
			free(name);
			return (0);
		}
		i++;
	}
	new_env[env_count] = ft_strdup(var);
	if (!new_env[env_count])
	{
		free_str_array(new_env, env_count);
		free(name);
		return (0);
	}
	free_str_array(ctx->env, env_count);
	ctx->env = new_env;
	free(name);
	return (1);
}

// checks for valid enviornment variables
int 	is_valid_identifier(const char *str)
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

// remove an enviornment variable
int	remove_env(t_context *ctx, const char *name)
{
	if (!name || !*name || !ctx || !ctx->env)
		return (0);
	int	env_count = 0;
	while (ctx->env[env_count])
		env_count++;
	int found = -1;
	int	i = 0;
	while (ctx->env[i])
	{
		if (ft_strncmp(ctx->env[i], name, ft_strlen(name)) == 0 && (ctx->env[i][ft_strlen(name)] == '=' || ctx->env[i][ft_strlen(name)] == '\0'))
		{
			found = i;
			break;
		}
		i++;
	}
	if (found == -1)
		return (1); // variable not found, no error
	free(ctx->env[found]);
	i = found;
	while(i < env_count - 1)
	{
		ctx->env[i] = ctx->env[i + 1];
		i++;
	}
	ctx->env[env_count - 1] = NULL;
	return (1);
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
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (env[i] + name_len + 1);
		i++;
	}
	return (NULL);
}