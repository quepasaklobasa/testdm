/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/20 17:05:46 by airupert          #+#    #+#             */
/*   Updated: 2025/07/20 17:57:14 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// extra util function used in get_command_path()
char	*ft_strtok(char *str, const char *delim)
{
	static char *next_token = NULL;
	char		*token;
	const char	*d;
	char		*start;

	if (str != NULL)
		next_token = str;
	else if (next_token == NULL || *next_token == '\0')
		return (NULL);
	// skip leading delimiters
	start = next_token;
	while(*start)
	{
		d = delim;
		while (*d)
		{
			if (*start == *d)
			{
				start++;
				break ;
			}
			d++;
		}
		if (*d == '\0')
			break ;
	}
	// if end of string, no more tokens
	if (*start == '\0')
	{
		next_token = NULL;
		return (NULL);
	}
	// set the token start
	token = start;
	// find the end of the token
	while (*next_token)
	{
		d = delim;
		while (*d)
		{
			if (*next_token == *d)
			{
				*next_token = '\0';
				next_token++;
				return (token);
			}
			d++;
		}
		next_token++;
	}
	// if reach here, this is last token
	if (*token)
		return (token);
	next_token = NULL;
	return (NULL);
}
