/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:31:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/28 18:29:23 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Initialize command list
CommandList *init_command_list(t_shell *shell)
{
	CommandList *list;
	
	if (!g_current_token || g_current_token->token.type == TOKEN_END)
		return (shell->exit_status = 2, NULL);
	list = malloc(sizeof(CommandList));
	if (!list)
	{
		write(2, "minishell: malloc: cannot allocate memory\n", 41);
		return (NULL);
	}
	list->cmd = parse_command(shell);
	list->next = NULL;
	if (!list->cmd)
		return (free(list), NULL);
	return (list);
}

// Handle pipe in command list
CommandList *handle_pipe(CommandList *list, t_shell *shell)
{
	if (consume(TOKEN_PIPE))
	{
		list->next = parse_program(g_current_token, shell); // Use g_current_token
		if (!list->next)
		{
			write(2, "minishell: syntax error near '|' \n", 32);
			free_command_list(list);
			return (NULL);
		}
	}
	return (list);
}

// Parse program: command_list
CommandList *parse_program(TokenNode *token_stream, t_shell *shell)
{
	CommandList *list;
	g_tokens = token_stream;
	g_current_token = token_stream;
	list = init_command_list(shell);
	if (!list)
		return (NULL);
	list = handle_pipe(list, shell);
	if (!list)
		return (NULL);
	if (g_current_token && g_current_token->token.type != TOKEN_END)
	{
		write(2, "minishell: syntax error: unexpected tokens\n", 42);
		free_command_list(list);
		return (NULL);
	}
	return (list);
}

// Parse command_list: ( command ( PIPE command )* )
CommandList *parse_command_list(t_shell *shell)
{
	CommandList *list;
	list = init_command_list(shell);
	if (!list)
		return (NULL);
	if (consume(TOKEN_PIPE))
	{
		list->next = parse_command_list(shell);
		if (!list->next)
		{
			write(2, "minishell: syntax error near '|'\n", 32);
			free_command_list(list);
			return (NULL);
		}
	}
	return (list);
}

// Free command list
void free_command_list(CommandList *list)
{
	CommandList *tmp;
	while (list)
	{
		tmp = list->next;
		if (list->cmd)
			free_command(list->cmd);
		free(list);
		list = tmp;
	}
}
