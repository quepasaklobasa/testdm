/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_operators.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:38:26 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/17 20:42:26 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Determine operator type and increment
void lexer_operator_type(char *input, int *i, TokenType *type, int *inc)
{
    if (input[*i] == '|')
    {
        *type = TOKEN_PIPE;
        *inc = 1; // Advance past the single pipe char
    }
    else if (input[*i] == '<' && input[*i + 1] == '<')
    {
        *type = TOKEN_HEREDOC;
        *inc = 2; // advance past 2 chars
    }
    else if (input[*i] == '>' && input[*i + 1] == '>')
    {
        *type = TOKEN_REDIRECT_APPEND;
        *inc = 2; // advance past 2 chars
    }
    else if (input[*i] == '<')
    {
        *type = TOKEN_REDIRECT_IN;
        *inc = 1;
    }
    else if (input[*i] == '>')
	{
		*type = TOKEN_REDIRECT_OUT;
		*inc = 1;
	}
        
}

// Assign operator token value
TokenNode *lexer_operator_value(TokenNode *tokens, int count, TokenType type)
{
    Token new_token;
    (void)count;
    new_token.type = type;
    new_token.value = NULL;
    tokens = append_token(tokens, new_token);
    if (!tokens)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    return (tokens);
}
