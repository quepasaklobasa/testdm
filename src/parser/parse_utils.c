/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 23:34:05 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:29:30 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

char	*expand_variables_in_string(char *str, t_shell *shell)
{
	char *result;
	char *temp;
	int i;
	int start;

	result = ft_strdup("");
	if (result == NULL)
	{
		return NULL;
	}
	i = 0;
	start = 0;
	while (str[i] != '\0')
	{
		if (str[i] == '$' && (ft_isalpha(str[i + 1]) || str[i + 1] == '_' || str[i + 1] == '?'))
		{
			temp = ft_substr(str, start, i - start);
			if (temp == NULL)
			{
				free(result);
				return NULL;
			}
			result = ft_strjoin_free(result, temp);
			free(temp);
			if (result == NULL)
			{
				return NULL;
			}
			i++; /* Skip $ */
			if (str[i] == '?')
			{
				temp = ft_itoa(shell->exit_status);
				if (temp == NULL)
				{
					free(result);
					return NULL;
				}
				result = ft_strjoin_free(result, temp);
				free(temp);
				if (result == NULL)
				{
					return NULL;
				}
				i++;
			}
			else
			{
				start = i;
				while (str[i] != '\0' && (ft_isalnum(str[i]) || str[i] == '_'))
				{
					i++;
				}
				temp = ft_substr(str, start, i - start);
				if (temp == NULL)
				{
					free(result);
					return NULL;
				}
				char *var_value = get_env_value(shell->env, temp);
				result = ft_strjoin_free(result, var_value);
				if (result == NULL)
				{
					free(temp);
					return NULL;
				}
				free(temp);
			}
			start = i;
		}
		else
		{
			i++;
		}
	}
	temp = ft_substr(str, start, i - start);
	if (temp == NULL)
	{
		free(result);
		return NULL;
	}
	result = ft_strjoin_free(result, temp);
	free(temp);
	return result;
}

char	*ft_strjoin_free(char *s1, char *s2)
{
	char *result;

	result = NULL;
	if (s1 == NULL && s2 == NULL)
	{
		result = NULL; 
	}
	else if (s1 == NULL)
	{
		if (s2 != NULL)
			result = ft_strdup(s2);
	}
	else if (s2 == NULL)
	{
		result = ft_strdup(s1);
		free(s1);
	}
	else
	{
		result = ft_strjoin(s1, s2);
		free(s1);
	}

	return result;
}
