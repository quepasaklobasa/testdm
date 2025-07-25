/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:35:30 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/24 21:19:11 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Initialize token list
TokenNode *lexer_init(void)
{
    TokenNode *tokens;
    tokens = malloc(sizeof(TokenNode));
    if (!tokens)
        return (NULL);
    tokens->token.type = TOKEN_END;
    tokens->token.value = NULL;
    tokens->next = NULL;
    return (tokens);
}

// Handle operators (<, >, >>, |)
TokenNode *lexer_operator(char *input, int *i, int *count, TokenNode *tokens)
{
    TokenType type;
    int inc;
    type = TOKEN_END;
    inc = 0;
    lexer_operator_type(input, i, &type, &inc);
    tokens = lexer_operator_value(tokens, *count, type);
    if (!tokens)
        return (NULL);
    *count += 1;
    *i += inc;
    return (tokens);
}

// Handle single-quoted strings
TokenNode *lexer_single_quote(char *input, int *i, int *count, TokenNode *tokens)
{
    int start;
	Token	new_token;
    start = *i;
	(*i)++;
    while (input[*i] && input[*i] != '\'')
        (*i)++;
    if (!input[*i])
    {
        write(2, "minishell: syntax error: unclosed single quote\n", 46);
        free_tokens(tokens);
        return (NULL);
    }
	new_token.type = TOKEN_WORD;
	new_token.value = ft_substr(input, start + 1, *i - start - 1);
	if (!new_token.value)
	{
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		free_tokens(tokens);
		return (NULL);
	}
	printf("DEBUG: lexer_single_quote processing '%.*s'\n", *i - start, input + start);
    tokens = append_token(tokens, new_token);
    if (!tokens)
	{
		free(new_token.value);
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		return (NULL);
	}
	(*count)++;
    (*i)++;
	printf("DEBUG: lexer_single_quote done, i=%d\n", *i);
    return (tokens);
}

// Handle double-quoted strings
TokenNode *lexer_double_quote(char *input, int *i, int *count, TokenNode *tokens)
{
    int start;
	Token	new_token;
    start = *i;
	(*i)++;
    while (input[*i] && input[*i] != '"')
        (*i)++;
    if (!input[*i])
    {
        write(2, "minishell: syntax error: unclosed double quote\n", 46);
        free_tokens(tokens);
        return (NULL);
    }
	new_token.type = TOKEN_WORD;
	new_token.value = ft_substr(input, start + 1, *i - start - 1);
	if (!new_token.value)
	{
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		free_tokens(tokens);
		return (NULL);
	}
	printf("DEBUG: lexer_double_quote processing '%.*s'\n", *i - start, input + start);
    tokens = append_token(tokens, new_token);
    if (!tokens)
	{
		free(new_token.value);
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		return (NULL);
	}
	(*count)++;
    (*i)++;
	printf("DEBUG: lexer_double_quote done, i=%d\n", *i);
    return (tokens);
}

// Main lexer function
TokenNode *lexer(char *input)
{
    int i;
    int count;
    TokenNode	*tokens;
	TokenNode	*current;
    i = 0;
    count = 0;
    tokens = NULL;
    while (input[i])
    {
        while (input[i] == ' ' || input[i] == '\t')
            i++;
        if (!input[i])
            break;
		printf("DEBUG: lexer processing input at i=%d, char='%c'\n", i, input[i]); // issue here
		/*if ((input[i] == '\'' || input[i] == '"') && tokens && i > 0 && \
		input[i - 1] != ' ' && input[i - 1] != '\t')
		{
			write (2, "minishell: command not found\n", 29);
			free_tokens(tokens);
			return (NULL);
		}*/
        tokens = lexer_process(input, &i, &count, tokens);
        if (!tokens)
            return (NULL);
		if (!input[i])
			break ;
    }
    if (count == 0)
    {
        tokens = lexer_init();
        return (tokens);
    }
	// append TOKEN_END only if not present
	if (tokens)
	{
		current = tokens;
		while (current->next)
			current = current->next;
		if (current->token.type != TOKEN_END)
			tokens = append_token(tokens, (Token){TOKEN_END, NULL});
	}
	printf("DEBUG: lexer done, count=%d\n", count);
    return (tokens);
}
