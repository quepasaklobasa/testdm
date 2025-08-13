/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 13:36:54 by airupert          #+#    #+#             */
/*   Updated: 2025/08/13 22:37:42 by jcouto           ###   ########.fr       */
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

// Update or add an enviornment variable
int update_env(t_shell *shell, const char *var)
{
    char *name_end;
    char *name;
    size_t name_len;
    int i;
    int env_count;

    if (!shell || !var || !shell->env)
        return (0);
    name_end = ft_strchr(var, '=');
    if (!name_end || name_end == var)
        return (0);
    name_len = name_end - var;
    name = ft_substr(var, 0, name_len);
    if (!name)
        return (0);
    i = 0;
    while (shell->env[i])
    {
        if (ft_strncmp(shell->env[i], name, name_len) == 0 && shell->env[i][name_len] == '=')
        {
            free(shell->env[i]);
            shell->env[i] = ft_strdup(var);
            free(name);
            if (!shell->env[i])
                return (0);
            return (1);
        }
        i++;
    }
    env_count = i;
    if (env_count >= 100) // Fixed-size array limit
    {
        write(STDERR_FILENO, "minishell: env: too many environment variables\n", 46);
        free(name);
        return (0);
    }
    shell->env[env_count] = ft_strdup(var);
    if (!shell->env[env_count])
    {
        free(name);
        return (0);
    }
    shell->env[env_count + 1] = NULL; // Null-terminate the array
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
void	remove_env(t_shell *shell, const char *name)
{
	int	i;
	int found = -1;
	size_t name_len;

	if (!name || !*name || !shell || !shell->env)
		return ;
	name_len = ft_strlen(name);
	i = 0;
	while (shell->env[i])
	{
		if (ft_strncmp(shell->env[i], name, name_len) == 0 && (shell->env[i][name_len] == '=' || shell->env[i][name_len] == '\0'))
		{
			found = i;
			break;
		}
		i++;
	}
	if (found == -1)
		return ; // variable not found, no error
	free(shell->env[found]);
	while (shell->env[found + 1])
	{
		shell->env[found] = shell->env[found + 1];
		found++;
	}
	shell->env[found] = NULL;
}

// helper to retrieve the value of an env variable
char *get_env_value(char **env, const char *name)
{
    int i;
    size_t name_len;

    if (!env[0] || !name)
        return (NULL);
    name_len = ft_strlen(name);
    i = 0;
    while (env[i][0])
    {
        if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
            return (env[i] + name_len + 1);
        i++;
    }
    return (NULL);
}
