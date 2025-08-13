/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_utils.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/23 15:16:58 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/13 22:50:42 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

TokenNode *create_token(TokenType type, char *value)
{
    TokenNode *new_node;
    
    new_node = malloc(sizeof(TokenNode));
    if (!new_node)
    {
        write(STDERR_FILENO, "minishell: malloc: cannot allocate memory\n", 41);
        if (value)
            free(value);
        return (NULL);
    }
    new_node->token.type = type;
    new_node->token.value = value;
    new_node->next = NULL;
    return (new_node);
}
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
