/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:35:30 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/28 19:35:22 by jcouto           ###   ########.fr       */
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
		printf("DEBUG: lexer at i=%d, char='%c'\n", i, input[i]);
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
	
	printf("DEBUG: lexer done, count=%d\n", count);
    return (tokens);
}
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
}/* ************************************************************************** */
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
