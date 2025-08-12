/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer_super_word.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/28 18:48:25 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/12 10:37:57 by jcouto           ###   ########.fr       */
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

static char *read_double_quote_fragment(const char *input, int *i, TokenNode **tokens, int *count)
{
    int start;
    char *frag;
    char *buffer;
    TokenNode *new_token;

    start = ++(*i);
    buffer = ft_calloc(1, 1);
    if (!buffer)
        return (NULL);
    while (input[*i] && input[*i] != '"')
    {
        if (input[*i] == '$' && (ft_isalpha(input[*i + 1]) || input[*i + 1] == '_' || input[*i + 1] == '?'))
        {
            if (*i > start)
            {
                frag = ft_substr(input, start, *i - start);
                if (!frag)
                    return (free(buffer), NULL);
                buffer = append_fragment(buffer, frag);
                free(frag);
                new_token = create_token(TOKEN_WORD, ft_strdup(buffer));
                if (!new_token)
                    return (free(buffer), NULL);
                printf("DEBUG: Creating TOKEN_WORD '%s'\n", buffer);
                *tokens = append_token(*tokens, new_token->token);
                (*count)++;
                free(buffer);
                buffer = ft_calloc(1, 1);
            }
            (*i)++;
            start = *i;
            while (input[*i] && (ft_isalnum(input[*i]) || input[*i] == '_'))
                (*i)++;
            if (*i > start)
            {
                frag = ft_substr(input, start, *i - start);
                if (!frag)
                    return (free(buffer), NULL);
                new_token = create_token(TOKEN_VARIABLE, frag);
                if (!new_token)
                    return (free(buffer), NULL);
                printf("DEBUG: Creating TOKEN_VARIABLE '%s'\n", frag);
                *tokens = append_token(*tokens, new_token->token);
                (*count)++;
                start = *i;
            }
            else if (input[start - 1] == '$' && input[start] == '?')
            {
                new_token = create_token(TOKEN_VARIABLE, ft_strdup("?"));
                if (!new_token)
                    return (free(buffer), NULL);
                printf("DEBUG: Creating TOKEN_VARIABLE '?'");
                *tokens = append_token(*tokens, new_token->token);
                (*count)++;
                start = ++(*i);
            }
            continue;
        }
        (*i)++;
    }
    if (!input[*i])
        return (printf("unclosed double quote"), free(buffer), NULL);
    if (*i > start)
    {
        frag = ft_substr(input, start, *i - start);
        if (!frag)
            return (free(buffer), NULL);
        buffer = append_fragment(buffer, frag);
        free(frag);
    }
    (*i)++;
    if (buffer[0])
    {
        new_token = create_token(TOKEN_WORD, buffer);
        if (!new_token)
            return (NULL);
        printf("DEBUG: Creating TOKEN_WORD '%s'\n", buffer);
        *tokens = append_token(*tokens, new_token->token);
        (*count)++;
    }
    else
        free(buffer);
    return ((char *)*tokens);
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
            frag = read_single_quote_fragment(input, i);
        else if (input[*i] == '"')
        {
            frag = read_double_quote_fragment(input, i, &tokens, count);
            if (!frag)
            {
                free(buffer);
                free_tokens(tokens);
                return (NULL);
            }
            continue;
        }
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
