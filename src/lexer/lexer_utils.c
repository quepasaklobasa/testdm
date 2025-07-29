/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:16:58 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/28 19:09:55 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Free token linked list
void free_tokens(TokenNode *tokens)
{
    TokenNode *tmp;
    while (tokens)
    {
        tmp = tokens->next;
        if (tokens->token.value)
            free(tokens->token.value);
        free(tokens);
        tokens = tmp;
    }
}

// Append a token to the linked list
TokenNode *append_token(TokenNode *tokens, Token token)
{
    TokenNode *new_node;
    TokenNode *current;
    new_node = malloc(sizeof(TokenNode));
    if (!new_node)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        free_tokens(tokens);
        return (NULL);
    }
    new_node->token = token;
    new_node->next = NULL;
    if (tokens && tokens->token.type == TOKEN_END && tokens->next == NULL)
    {
        free(tokens);
        return (new_node);
    }
    if (!tokens)
        return (new_node);
    current = tokens;
    while (current->next)
        current = current->next;
    current->next = new_node;
    return (tokens);
}
