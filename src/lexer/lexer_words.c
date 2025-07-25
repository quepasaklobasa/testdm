/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:36:52 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/24 21:25:17 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Handle word tokens
TokenNode *lexer_word(char *input, int *i, int *count, TokenNode *tokens)
{
    int	start;
    int	is_export_arg = 0;
    Token new_token;
    start = *i;
	// Check if arg is after export command
	if (*count > 0 && tokens && tokens->token.type == TOKEN_WORD && \
		ft_strcmp(tokens->token.value, "export")  == 0)
		is_export_arg = 1;
	if (is_export_arg)
	{
		while (input[*i] && (input[*i] != ' ' || (*i > start && input[*i - 1] == '=')) && \
		lexer_classify(input, *i) == 0)
		(*i)++;
	}
	else
	{
		 while (input[*i] && lexer_classify(input, *i) == 0)
        (*i)++;
	}
	if (*i == start)
		return (tokens);
    new_token.type = TOKEN_WORD;
    new_token.value = ft_substr(input, start, *i - start);
    if (!new_token.value)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        free_tokens(tokens);
        return (NULL);
    }
    printf("DEBUG: lexer_word creating token '%s'\n", new_token.value);
    tokens = append_token(tokens, new_token);
    if (!tokens)
    {
        free(new_token.value);
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    (*count)++;
    return (tokens);
}

// Classify character type
int lexer_classify(char *input, int i)
{
    if (input[i] == ' ' || input[i] == '\0')
        return (-1);
    if (input[i] == '|' || input[i] == '<' || input[i] == '>')
        return (1);
    if (input[i] == '\'')
        return (2);
    if (input[i] == '"')
        return (3);
    if (input[i] == '$')
        return (4);
    return (0);
}

// Handle variable tokens
TokenNode *lexer_variable(char *input, int *i, int *count, TokenNode *tokens)
{
    int start;
    Token new_token;
    start = *i;
    (*i)++;
    while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '?' || input[*i] == '_'))
        (*i)++;
    new_token.type = TOKEN_VARIABLE;
    new_token.value = ft_substr(input, start, *i - start);
    if (!new_token.value)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        free_tokens(tokens);
        return (NULL);
    }
    tokens = append_token(tokens, new_token);
    if (!tokens)
    {
        free(new_token.value);
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    (*count)++;
    return (tokens);
}
