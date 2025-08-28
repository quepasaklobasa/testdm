/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:19:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:27:31 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

static char *process_token(TokenNode *token, t_shell *shell);
static void cleanup_args(char **args, int count);

// Consume a token of expected type
int consume(TokenType type)
{

	if (g_current_token && g_current_token->token.type == type)
	{
		g_current_token = g_current_token->next;
		return (1);
	}
	return (0);
}

// Allocate memory for args array
char	**parse_args_alloc(int *arg_count)
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
char	**parse_args_process(char **args, int *arg_count, t_shell *shell)
{
	int i;

	i = 0;
	while (g_current_token != NULL)
	{
		if (g_current_token->token.type != TOKEN_WORD && g_current_token->token.type != TOKEN_VARIABLE)
		{
			break;
		}
		args[i] = process_token(g_current_token, shell);
		if (args[i] == NULL)
		{
			cleanup_args(args, i);
			return NULL;
		}
		g_current_token = g_current_token->next;
		i++;
	}
	args[i] = NULL;
	*arg_count = i;
	return args;
}

// Parse arguments
char	**parse_args(int *arg_count, t_shell *shell)
{
	char **args;
	*arg_count = 0;
	args = parse_args_alloc(arg_count);
	if (!args)
		return (NULL);
	args = parse_args_process(args, arg_count, shell);
	return (args);
}

// Handle redirection type
int	parse_redirection_type(Command *cmd, TokenType type)
{
	char *value;
	if (!g_current_token || (g_current_token->token.type != TOKEN_WORD && 
		g_current_token->token.type != TOKEN_VARIABLE))
		return (1);
	value = ft_strdup(g_current_token->token.value);
	if (!value)
	{
		write(STDERR_FILENO, "minishell: malloc error \n", 24);
		return (1);
	}
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
static char	*process_token(TokenNode *token, t_shell *shell)
{
	char *result;
	char *value;

	result = NULL;
	if (token->token.type == TOKEN_VARIABLE)
	{
		if (ft_strcmp(token->token.value, "$?") == 0)
			{
				result = ft_itoa(shell->exit_status);
			}
		else
		{
			value = get_env_value(shell->env, token->token.value + 1);
			if (value == NULL)
			{
				result = ft_strdup("");
			}
			else
			{
				result = ft_strdup(value);
			}
			if (result == NULL)
			{
				result = ft_strdup("");
			}
		}
	}
	else /* TOKEN_WORD */
	{
		if (token->token.value == NULL)
		{
			return NULL;
		}
		if (ft_strchr(token->token.value, '$'))
		{
			result = expand_variables_in_string(token->token.value, shell);
		}
		else
		{
			result = ft_strdup(token->token.value);
		}
	}
	return result;
}

/* Cleans up allocated arguments on error */
static void	cleanup_args(char **args, int count)
{
	while (count > 0)
	{
		count--;
		free(args[count]);
	}
	free(args);
	write(STDERR_FILENO, "minishell: malloc: cannot allocate memory\n", 42);
}
