/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_words.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:36:52 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/28 18:57:11 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Classify character type
int lexer_classify(char *input, int i)
{
    if (input[i] == ' ' || input[i] == '\t' || input[i] == '\0')
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
