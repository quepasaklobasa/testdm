/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:35:30 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:29:00 by jcouto           ###   ########.fr       */
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
		if (lexer_classify(input, i) == 1) // operator
			tokens = lexer_operator(input, &i, &count, tokens);
		else if (lexer_classify(input, i) == 4) // variable $
			tokens = lexer_variable(input, &i, &count, tokens);
		else // word + embedded quotes
			tokens = lexer_word_combined(input, &i, &count, tokens);
		if (!tokens)
			return (NULL);
	}
	if (count == 0)
		return lexer_init();
	current = tokens;
	while (current->next)
		current = current->next;
	if (current->token.type != TOKEN_END)
		tokens = append_token(tokens, (Token){TOKEN_END, NULL});
	return (tokens);
}
