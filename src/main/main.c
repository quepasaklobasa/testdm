/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 17:28:32 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/21 16:45:38 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Global variables for signal handling and parser
volatile sig_atomic_t g_signal;
TokenNode *g_tokens;
TokenNode *g_current_token;

// Debug: Print token list
static void print_tokens(TokenNode *tokens)
{
    int i;
    i = 0;
    while (tokens)
    {
        const char *type_str;
        switch (tokens->token.type)
        {
            case TOKEN_WORD: type_str = "TOKEN_WORD"; break;
            case TOKEN_VARIABLE: type_str = "TOKEN_VARIABLE"; break;
            case TOKEN_PIPE: type_str = "TOKEN_PIPE"; break;
            case TOKEN_REDIRECT_IN: type_str = "TOKEN_REDIRECT_IN"; break;
            case TOKEN_REDIRECT_OUT: type_str = "TOKEN_REDIRECT_OUT"; break;
            case TOKEN_REDIRECT_APPEND: type_str = "TOKEN_REDIRECT_APPEND"; break;
            case TOKEN_HEREDOC: type_str = "TOKEN_HEREDOC"; break;
            case TOKEN_END: type_str = "TOKEN_END"; break;
            default: type_str = "UNKNOWN"; break;
        }
        printf("  [%d]: type=%s, value=%s\n", i++, type_str, 
               tokens->token.value ? tokens->token.value : "(null)");
        tokens = tokens->next;
    }
}

// Debug: Print command list
static void print_command_list(CommandList *cmd_list)
{
    int cmd_idx;
    cmd_idx = 0;

    while (cmd_list)
    {
        Command *cmd;
        cmd = cmd_list->cmd;
        printf("Command %d:\n", cmd_idx++);
        if (cmd->cmd)
            printf("  cmd: %s (from args[0], TOKEN_WORD)\n", cmd->cmd);
        if (cmd->args && cmd->args[1])
        {
            printf("  args:\n");
            for (int i = 1; cmd->args[i] || i == 1; i++)
            {
                const char *role;
                role = (cmd->args[i] && cmd->args[i][0] == '-') ? "option" : "argument";
                printf("    [%d]: %s (TOKEN_WORD, %s)\n", i - 1, cmd->args[i] ? cmd->args[i] : "(null)", role);
            }
        }
        if (cmd->redirect_in)
            printf("  redirect_in: %s\n", cmd->redirect_in);
        cmd_list = cmd_list->next;
    }
}

// Main helper: Process input
void process_input(char *line, t_shell *shell)
{
	TokenNode	*tokens;
    CommandList	*cmd_list;
    t_context	ctx;
	int			i;
	
	i = 0;
    tokens = lexer(line);
    ctx.pipefd = NULL;
    // ctx.env = shell->env; // removed from original to solve ctx->env update issue
    // ctx.exit_status = 0;
	// copy shell->env to ctx.env
	while (shell->env && shell->env[i])
		i++;
	ctx.env = ft_calloc(i + 1, sizeof(char *));
	if (!ctx.env)
	{
		free_tokens(tokens);
		return ;
	}
	i = 0;
	while (shell->env && shell->env[i])
	{
		ctx.env[i] = ft_strdup(shell->env[i]);
		if (!ctx.env[i])
		{
			while (i > 0)
				free(ctx.env[--i]);
			free(ctx.env);
			free_tokens(tokens);
			return ;
		}
		i++;
	}
	ctx.exit_status = shell->exit_status;
    printf("Tokens:\n");
    print_tokens(tokens);
    cmd_list = parse_program(tokens, &ctx);
    if (cmd_list)
    {
        print_command_list(cmd_list);
        execute_command_list(cmd_list, &ctx, shell);
		shell->exit_status = ctx.exit_status;
		if (shell->env)
		{
			i = 0;
			while (shell->env[i])
				free(shell->env[i++]);
			free(shell->env);
		}
		shell->env = ctx.env;
		ctx.env = NULL; // prevent double-free
        free_command_list(cmd_list);
    }
	else
	{
		// clean up ctx.env if parsing failed
		if (ctx.env)
		{
			i = 0;
			while (ctx.env[i])
				free(ctx.env[i++]);
			free(ctx.env);
		}
		shell->exit_status = 1;
	}
    free_tokens(tokens);
}

// Main helper: Run shell loop
int run_shell_loop(t_shell *shell)
{
    char *line;
    while (1)
    {
        if (shell->exit_flag || g_signal == SIGINT)
        {
			// exit status updated for ctrl+c
			if (g_signal == SIGINT)
				shell->exit_status = 128 + SIGINT;
            g_signal = 0;
            if (shell->exit_flag)
                return (0);
            continue;
        }
        line = readline("minishell> ");
        if (!line)
        {
            printf("exit\n");
            return (0);
        }
        if (*line)
        {
            add_history(line);
            process_input(line, shell);
        }
        free(line);
    }
    return (0);
}

// Main function
int main(int ac, char **av, char **env)
{
    t_shell shell;
    (void)ac;
    (void)av;
    if (init_shell(&shell, env) != 0)
    {
        printf("minishell: initialization failed\n");
        return (EXIT_FAILURE);
    }
    run_shell_loop(&shell);
    cleanup_shell(&shell);
    return (0);
}
