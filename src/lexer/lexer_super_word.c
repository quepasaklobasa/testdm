/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_super_word.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:48:25 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/13 22:56:04 by jcouto           ###   ########.fr       */
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

static int read_double_quote_fragment(const char *input, int *i, TokenNode **tokens, int *count, char **buffer_ptr)
{
    int start;
    char *frag;
    char *buffer = *buffer_ptr;
    TokenNode *new_token;

    (*i)++; // Skip opening quote
    start = *i;
    
    while (input[*i] && input[*i] != '"')
    {
        if (input[*i] == '$' && (ft_isalpha(input[*i + 1]) || input[*i + 1] == '_' || input[*i + 1] == '?'))
        {
            // Add any text before the variable to buffer
            if (*i > start)
            {
                frag = ft_substr(input, start, *i - start);
                if (!frag)
                    return (free(buffer), -1);
                buffer = append_fragment(buffer, frag);
                free(frag);
            }
            
            // Create variable token
            (*i)++; // Skip $
            start = *i;
            
            if (input[*i] == '?')
            {
                (*i)++;
                new_token = create_token(TOKEN_VARIABLE, ft_strdup("$?"));
            }
            else
            {
                while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
                    (*i)++;
                frag = ft_substr(input, start - 1, *i - start + 1); // Include the $
                new_token = create_token(TOKEN_VARIABLE, frag);
            }
            
            if (!new_token)
                return (free(buffer), -1);
                
            printf("DEBUG: Creating TOKEN_VARIABLE '%s'\n", new_token->token.value);
            *tokens = append_token(*tokens, new_token->token);
            free(new_token); // We only needed it temporarily
            (*count)++;
            start = *i;
            continue;
        }
        (*i)++;
    }
    
    if (!input[*i])
        return (printf("unclosed double quote\n"), free(buffer), -1);
        
    // Add remaining text to buffer
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
            if (read_double_quote_fragment(input, i, &tokens, count, &buffer) < 0)
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
        printf("DEBUG: lexer_word_combined creating token '%s'\n", buffer);
        (*count)++;
        return (append_token(tokens, new_token));
    }
    
    free(buffer);
    return (tokens);
}
