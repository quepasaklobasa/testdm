/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:31:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/20 15:31:13 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Initialize command list
CommandList *init_command_list(t_context *ctx)
{
    CommandList *list;
    list = malloc(sizeof(CommandList));
    if (!list)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    list->cmd = parse_command(ctx);
    list->next = NULL;
    if (!list->cmd)
    {
        free(list);
        return (NULL);
    }
    return (list);
}

// Handle pipe in command list
// changing arguments for function
CommandList *handle_pipe(CommandList *list, t_context *ctx)
{
    if (consume(TOKEN_PIPE))
    {
        list->next = parse_program(g_current_token, ctx); // Use g_current_token
        if (!list->next)
        {
            write(2, "minishell: syntax error near '|'\n", 32);
            free_command_list(list);
            return (NULL);
        }
    }
    return (list);
}

// Parse program: command_list
CommandList *parse_program(TokenNode *token_stream, t_context *ctx)
{
    CommandList *list;
    g_tokens = token_stream;
    g_current_token = token_stream;
    list = init_command_list(ctx);
	char	*path;
    if (!list)
        return (NULL);
    // validate command checker (check for non-empty cmd)
    if (list->cmd->cmd && !is_builtin(list->cmd->cmd))
    {
		path = get_command_path(list->cmd->cmd, ctx);
		if (!path)
		{
			write(2, "minishell: command not found: ", 30);
			write(2, list->cmd->cmd, ft_strlen(list->cmd->cmd));
			write(2, "\n", 1);
			free_command_list(list);
			return (NULL);
		}
		free(path);
    }
    list = handle_pipe(list, ctx); // removed token stream
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
CommandList *parse_command_list(t_context *ctx)
{
    CommandList *list;
    list = init_command_list(ctx);
    if (!list)
        return (NULL);
    if (consume(TOKEN_PIPE))
    {
        list->next = parse_command_list(ctx);
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
