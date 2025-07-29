/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:19:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/25 19:34:15 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Consume a token of expected type
int consume(TokenType type)
{

    if (g_current_token && g_current_token->token.type == type)
    {
		printf("DEBUG: consume: consumed type=%d\n", type);
        g_current_token = g_current_token->next;
        return (1);
    }
    return (0);
}

// Allocate memory for args array
char **parse_args_alloc(int *arg_count)
{
    TokenNode   *temp;
	// char		*combined;
	int			len;

    temp = g_current_token;
	*arg_count = 0;
    while (temp && (temp->token.type == TOKEN_WORD || temp->token.type == TOKEN_VARIABLE))
    {
		if (temp->token.type == TOKEN_WORD && temp->token.value && temp->token.value[0] == '/')
		{
			len = 0;
			// combined = NULL;
			while (temp && temp->token.type == TOKEN_WORD && temp->token.value && temp->token.value[0] != ' ')
			{
				len += ft_strlen(temp->token.value);
				temp = temp->next;
			}
			(*arg_count)++;
		}
		else
		{
			(*arg_count)++;
			temp = temp->next;
		} 
    }
    return (malloc((*arg_count + 1) * sizeof(char *)));
}

// Process arguments into array
char **parse_args_process(char **args, int *arg_count, t_context *ctx)
{
    int		i;
    char	*value;
	char	*combined;
	int		len;
	char	*tmp;

    i = 0;
    while (g_current_token && (g_current_token->token.type == TOKEN_WORD || 
           g_current_token->token.type == TOKEN_VARIABLE))
    {
		// the parser expansion and modification for echo $?
		if (g_current_token->token.type == TOKEN_VARIABLE && g_current_token->token.value &&
			g_current_token->token.value[0] == '/')
		{
			len = 0;
			combined = NULL;
			while(g_current_token && g_current_token->token.type == TOKEN_WORD &&
				g_current_token->token.value && g_current_token->token.value[0] != ' ')
			{
				len += ft_strlen(g_current_token->token.value);
				if (!combined)
					combined = ft_strdup(g_current_token->token.value);
				else
				{
					tmp = ft_strjoin(combined, g_current_token->token.value);
					free(combined);
					combined = tmp;
				}
				g_current_token = g_current_token->next;
			}
			args[i] = combined;
			if (!args[i])
			{
				while(i > 0)
					free(args[--i]);
				free(args);
			}
			i++;
		}
		else
		{
			if (g_current_token->token.type == TOKEN_VARIABLE)
			{
				if (ft_strcmp(g_current_token->token.value, "$?" ) == 0)
				{
					args[i] = ft_itoa(ctx->exit_status);
					if (!args[i])
					{
						while(i > 0)
							free(args[--i]);
						free(args);
						write(2, "minishell: malloc: cannot allocate memory\n", 41);
						return (NULL);
					}
					printf("DEBUG: Expanded $? to '%s'\n", args[i]); // debugger
					g_current_token = g_current_token->next;
				}
				else
				{
					value = get_env_value(ctx->env, g_current_token->token.value + 1);
					args[i] = ft_strdup(value ? value : "");
					if (!args[i])
					{
						while (i > 0)
							free(args);
						write(2, "minishell: malloc: cannot allocate memory\n", 41);
						return (NULL);
					}
					printf("DEBUG: Expanded variable '%s' to '%s'\n", g_current_token->token.value, args[i]);
					g_current_token = g_current_token->next;
				}
			}
			else
			{
				if (!g_current_token->token.value)
				{
					write(2, "minishell: parse_args_process: null token value\n", 47);
					while (i > 0)
						free(args[--i]);
					return (free(args), NULL);
				}
				args[i] = ft_strdup(g_current_token->token.value);
				if (!args[i])
				{
					while(i > 0)
						free(args[--i]);
					return (free(args), NULL);
				}
				g_current_token = g_current_token->next;
			}
			i++;
		}
	}
	args[i] = NULL;
	*arg_count = i;
	return (args);
}

// Parse arguments
char **parse_args(int *arg_count, t_context *ctx)
{
    char **args;
    *arg_count = 0;
    args = parse_args_alloc(arg_count);
    if (!args)
        return (NULL);
    args = parse_args_process(args, arg_count, ctx);
    return (args);
}

// Handle redirection type
int parse_redirection_type(Command *cmd, TokenType type)
{
    char *value;
    if (!g_current_token || (g_current_token->token.type != TOKEN_WORD && 
        g_current_token->token.type != TOKEN_VARIABLE))
        return (-1);
    value = ft_strdup(g_current_token->token.value);
    if (!value)
        return (-1);
    g_current_token = g_current_token->next;
    if (type == TOKEN_REDIRECT_IN)
        cmd->redirect_in = value;
    else if (type == TOKEN_REDIRECT_OUT)
        cmd->redirect_out = value;
    else if (type == TOKEN_REDIRECT_APPEND)
        cmd->redirect_append = value;
    else if (type == TOKEN_HEREDOC)
        cmd->heredoc_delim = value;
    return (0);
}
