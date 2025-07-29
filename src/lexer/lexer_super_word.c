/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_super_word.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:48:25 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/28 19:41:22 by jcouto           ###   ########.fr       */
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
		return (NULL); // syntax error: unclosed '
	frag = ft_substr(input, start, *i - start);
	(*i)++; // skip closing '
	return (frag);
}

static char *read_double_quote_fragment(const char *input, int *i)
{
	int start;
	char *frag;

	start = ++(*i);
	while (input[*i] && input[*i] != '"')
		(*i)++;
	if (!input[*i])
		return (NULL); // syntax error: unclosed "
	frag = ft_substr(input, start, *i - start);
	(*i)++; // skip closing "
	return (frag);
}

static char *read_plain_fragment(char *input, int *i)
{
	int start = *i;

	while (input[*i] &&
		   input[*i] != '\'' &&
		   input[*i] != '"' &&
		   lexer_classify(input, *i) == 0)
		(*i)++;
	if (*i == start)
		return (NULL);
	return ft_substr(input, start, *i - start);
}

TokenNode *lexer_word_combined(char *input, int *i, int *count, TokenNode *tokens)
{
	char *buffer = ft_calloc(1, 1);
	char *frag;

	while (input[*i] && lexer_classify(input, *i) == 0)
	{
		if (input[*i] == '\'')
			frag = read_single_quote_fragment(input, i);
		else if (input[*i] == '"')
			frag = read_double_quote_fragment(input, i);
		else
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
	if (buffer[0] == '\0')
	{
		free(buffer);
		return tokens;  // Avoid creating empty token
	}
	Token new_token;
	new_token.type = TOKEN_WORD;
	new_token.value = buffer;
	printf("DEBUG: lexer_word_combined creating token '%s'\n", buffer);
	(*count)++;
	return (append_token(tokens, new_token));
}