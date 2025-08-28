/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_super_word.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:48:25 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:28:51 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char *append_fragment(char *buffer, char *frag)
{
	char *new_buf = ft_strjoin(buffer, frag);
	free(buffer);
	return (new_buf);
}

static char *read_single_quote_fragment(const char *input, int *i)
{
	int start;
	char *frag;

	start = ++(*i);
	while (input[*i] && input[*i] != '\'')
		(*i)++;
	if (!input[*i])
		return (printf("unclosed single quote"), NULL);
	frag = ft_substr(input, start, *i - start);
	(*i)++;
	return (frag);
}

static int read_double_quote_fragment(const char *input, int *i, char **buffer_ptr)
{
	int start;
	char *frag;
	char *buffer = *buffer_ptr;

	(*i)++;
	start = *i;
	while (input[*i] && input[*i] != '"')
		(*i)++;
	if (!input[*i])
		return (printf("unclosed double quote\n"), free(buffer), -1);
	// Add the entire quoted content to buffer (including variables like $VAR)
	if (*i > start)
	{
		frag = ft_substr(input, start, *i - start);
		if (!frag)
			return (free(buffer), -1);
		buffer = append_fragment(buffer, frag);
		free(frag);
	}
	(*i)++; // Skip closing quote
	*buffer_ptr = buffer;
	return (0); // Success
}

static char *read_plain_fragment(char *input, int *i)
{
	int start = *i;

	while (input[*i] && input[*i] != '\'' && input[*i] != '"' && lexer_classify(input, *i) == 0)
		(*i)++;
	if (*i == start)
		return (ft_strdup(""));
	return ft_substr(input, start, *i - start);
}

TokenNode *lexer_word_combined(char *input, int *i, int *count, TokenNode *tokens)
{
	char *buffer = ft_calloc(1, 1);
	char *frag;

	if (!buffer)
		return (NULL);
		
	while (input[*i] && lexer_classify(input, *i) != -1 && lexer_classify(input, *i) != 1)
	{
		if (input[*i] == '\'')
		{
			frag = read_single_quote_fragment(input, i);
			if (!frag)
			{
				free(buffer);
				free_tokens(tokens);
				return (NULL);
			}
			buffer = append_fragment(buffer, frag);
			free(frag);
		}
		else if (input[*i] == '"')
		{
			if (read_double_quote_fragment(input, i, &buffer) < 0)
			{
				free_tokens(tokens);
				return (NULL);
			}
		}
		else
		{
			frag = read_plain_fragment(input, i);
			if (!frag)
			{
				write(2, "minishell: lexer syntax error\n", 30);
				free(buffer);
				free_tokens(tokens);
				return (NULL);
			}
			buffer = append_fragment(buffer, frag);
			free(frag);
		}
	}
	
	if (buffer[0])
	{
		Token new_token;
		new_token.type = TOKEN_WORD;
		new_token.value = buffer;
		(*count)++;
		return (append_token(tokens, new_token));
	}
	free(buffer);
	return (tokens);
}
