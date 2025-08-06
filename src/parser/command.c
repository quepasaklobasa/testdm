/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:20:17 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/06 21:13:48 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Parse redirection for command
int parse_redirection(Command *cmd)
{
    TokenType type = TOKEN_END; // initialize to a default

    if (consume(TOKEN_REDIRECT_IN))
        type = TOKEN_REDIRECT_IN;
    else if (consume(TOKEN_REDIRECT_OUT))
        type = TOKEN_REDIRECT_OUT;
    else if (consume(TOKEN_REDIRECT_APPEND))
        type = TOKEN_REDIRECT_APPEND;
    else if (consume(TOKEN_HEREDOC))
        type = TOKEN_HEREDOC;
    
    if (type != TOKEN_END) // check if a redirection was consumed
    {
        printf("DEBUG: parse_redirection: consumed type=%d, next value=%s\n", 
               type, g_current_token ? g_current_token->token.value : "(null)");
        if (!g_current_token || (g_current_token->token.type != TOKEN_WORD && \
            g_current_token->token.type != TOKEN_VARIABLE))
            {
                write(2, "minishell: syntax error near redirection\n", 40);
                return (-1);
            }
        return (parse_redirection_type(cmd, type));
    }
    return (0);
}

// Initialize command structure
Command *parse_command_init(void)
{
    Command *cmd;
    cmd = malloc(sizeof(Command));
    if (!cmd)
    {
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    cmd->cmd = NULL;
    cmd->args = NULL;
    cmd->redirect_in = NULL;
    cmd->redirect_out = NULL;
    cmd->redirect_append = NULL;
    cmd->heredoc_delim = NULL;
    cmd->in_fd = STDIN_FILENO;
    cmd->out_fd = STDOUT_FILENO;
    cmd->pid = 0;
    return (cmd);
}

// Process command arguments
Command *parse_command_args(Command *cmd, int *arg_count, t_shell *shell)
{
    cmd->args = parse_args(arg_count, shell);
    if (!cmd->args)
    {
        free_command(cmd);
        return (NULL);
    }
    if (*arg_count > 0)
        cmd->cmd = ft_strdup(cmd->args[0]);
    if (*arg_count > 0 && !cmd->cmd)
    {
        free_command(cmd);
        write(2, "minishell: malloc: cannot allocate memory\n", 41);
        return (NULL);
    }
    return (cmd);
}

// Parse a single command
Command *parse_command(t_shell *shell)
{
    Command *cmd;
    int arg_count;
    cmd = parse_command_init();
    if (!cmd)
        return (NULL);
    cmd = parse_command_args(cmd, &arg_count, shell);
    if (!cmd)
        return (NULL);
    while (g_current_token && (g_current_token->token.type == TOKEN_REDIRECT_IN ||
           g_current_token->token.type == TOKEN_REDIRECT_OUT ||
           g_current_token->token.type == TOKEN_REDIRECT_APPEND ||
           g_current_token->token.type == TOKEN_HEREDOC))
    {
        if (parse_redirection(cmd) != 0)
        {
            free_command(cmd);
            return (NULL);
        }
    }
    return (cmd);
}

// Free command structure
void free_command(Command *cmd)
{
    int i;
    if (!cmd)
        return;
    if (cmd->cmd)
        free(cmd->cmd);
    if (cmd->args)
    {
        i = 0;
        while (cmd->args[i])
            free(cmd->args[i++]);
        free(cmd->args);
    }
    if (cmd->redirect_in)
        free(cmd->redirect_in);
    if (cmd->redirect_out)
        free(cmd->redirect_out);
    if (cmd->redirect_append)
        free(cmd->redirect_append);
    if (cmd->heredoc_delim)
        free(cmd->heredoc_delim);
    free(cmd);
}
