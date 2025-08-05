/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:10 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/04 20:40:55 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int is_builtin(const char *cmd)
{
    if (!cmd)
        return (0);
    if (ft_strcmp(cmd, "echo") == 0 || ft_strcmp(cmd, "cd") == 0 \
        || ft_strcmp(cmd, "pwd") == 0 || ft_strcmp(cmd, "export") == 0 \
        || ft_strcmp(cmd, "unset") == 0 || ft_strcmp(cmd, "env") == 0 \
        || ft_strcmp(cmd, "exit") == 0)
        return (1);
    return (0);
}

static int	cmd_exit(Command *cmd, t_context *ctx, t_shell *shell)
{
	shell->exit_flag = 1;
	if (cmd->args[1])
	{
		if (cmd->args[2] || !is_numeric(cmd->args[1]))
		{
			write(STDERR_FILENO, "minishell: exit: numeric argument required\n", 43);
			ctx->exit_status = 2;
			return (2);
		}
		ctx->exit_status = ft_atoi(cmd->args[1]) % 256;
	}
	else
		ctx->exit_status = 0;
	return (ctx->exit_status);
}

static int	cmd_env(Command *cmd, t_context *ctx)
{
	int	i;

	if (!ctx || !ctx->env)
	{
		write(STDERR_FILENO, "minishell: env: invalid identifier\n", 31);
		return ((ctx->exit_status = 1), 1);
	}
	if (cmd->args[1])
	{
		write(STDERR_FILENO, "minishell: env: no options or arguments allowed\n", 48);
		return ((ctx->exit_status = 1), 1);
	}
	i = 0;
	while (ctx->env[i])
	{
		write(cmd->out_fd, ctx->env[i], ft_strlen(ctx->env[i]));
		write(cmd->out_fd, "\n", 1);
		i++;
	}
	ctx->exit_status = 0;
	return (0);
}

static int	cmd_unset(Command *cmd, t_context *ctx)
{
	int	i;
	int	error = 0;
	
	if (!cmd->args[1]) // unset with no arguments is valid in bash - just does nothing
		return ((ctx->exit_status = 0), 0);
	i = 1;
	while (cmd->args[i])
	{		// Check if it's a valid identifier first
		if (!is_valid_identifier(cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: unset: `", 19);
			write(STDERR_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
			write(STDERR_FILENO, "': not a valid identifier\n", 26);
			error = 1;
		}
		else	// remove_env now returns 0 on success, 1 on error
			remove_env(ctx, cmd->args[i]);
		i++;
	}
	ctx->exit_status = error;
	return (error);
}

static int	cmd_export(Command *cmd, t_context *ctx)
{
	int	i;
	int	error;

	error = 0;
	if (!ctx || !cmd)
		return ((write(STDERR_FILENO, "minishell: export: invalid context\n", 34)), 1);
	if (!cmd->args[1])
	{
		if (!ctx->env)
			return ((ctx->exit_status = 0), 0);
		i = 0;
		while (ctx->env[i])
		{
			write(cmd->out_fd, "declare -x ", 11); //debug?
			write(cmd->out_fd, ctx->env[i], ft_strlen(ctx->env[i]));
			write(cmd->out_fd, "\n", 1);
			i++;
		}
		ctx->exit_status = 0;
		return (0);
	}
	i = 1;
	while (cmd->args[i])
	{
		if (!is_valid_identifier(cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: export: `", 20);
			write(STDERR_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
			write(STDERR_FILENO, "`:not a valid identifier\n", 25);
			error = 1;
		}
		else if (!update_env(ctx, cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: export: failed to update environment\n", 48);
			error = 1;
		}
		i++;
	}
	ctx->exit_status = error;
	return (error);
}

static int	cmd_pwd(Command *cmd, t_context *ctx)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		write(cmd->out_fd, cwd, ft_strlen(cwd));
		write(cmd->out_fd, "\n", 1);
		ctx->exit_status = 0;
	}
	else
	{
		write(STDERR_FILENO, "minishell: pwd: error retrieving current directory\n", 50);
		ctx->exit_status = 1;
	}
	return (ctx->exit_status);
}

static int	cmd_cd(Command *cmd, t_context *ctx)
{
	char	*path;
	char	cwd[1024];
	char	*oldpwd;
	char	*oldpwd_str;
	char	*pwd_str;
	
	path = cmd->args[1];
	oldpwd = getcwd(cwd, sizeof(cwd));
	if (!path)
		path = getenv("HOME");
	if (!path || chdir(path) != 0)
	{
		write(STDERR_FILENO, "minishell: cd: invalid path\n", 28);
		ctx->exit_status = 1;
		return (1);
	}
	if (oldpwd)
	{
		oldpwd_str = ft_strjoin("OLDPWD=", oldpwd);
		if (oldpwd_str)
		{
			update_env(ctx, oldpwd_str);
			free(oldpwd_str);
		}
	}
	if (getcwd(cwd, sizeof(cwd)))
	{
		pwd_str = ft_strjoin("PWD=", cwd);
		if (pwd_str)
		{
			update_env(ctx, pwd_str);
			free(pwd_str);
		}
	}
	ctx->exit_status = 0;
	return (0);
}

static int	cmd_echo(Command *cmd, t_context *ctx)
{
	int	i;
	int	no_newline;

	i = 1;
	no_newline = 0;
	while (cmd->args[i] && ft_strcmp(cmd->args[i], "-n") == 0)
	{
		no_newline = 1;
		i++;
	}
	while (cmd->args[i])
	{
		if (write(cmd->out_fd, cmd->args[i], ft_strlen(cmd->args[i])) == -1)
		{
			write(STDERR_FILENO, "minishell: echo: write error\n", 29);
			ctx->exit_status = 1;
			return (1);
		}
		if (cmd->args[i + 1])
		{
			if (write(cmd->out_fd, " ", 1) == -1)
			{
				write(STDERR_FILENO, "minishell: echo: write error\n", 29);
				ctx->exit_status = 1;
				return (1);
			}
		}
		i++;
	}
	if (!no_newline)
	{
		if (write(cmd->out_fd, "\n", 1) == -1)
		{
			write(STDERR_FILENO, "mimshell: echo: write error \n", 29);
			ctx->exit_status = 1;
			return (1);
		}
	}
	ctx->exit_status = 0;
	return (0);
}

// Execute built-in command
int exec_builtin(Command *cmd, t_context *ctx, t_shell *shell) // t_shell added for exit flag access
{
	if (!cmd || !ctx || !shell)
		return (1);
    if (ft_strcmp(cmd->cmd, "echo") == 0) // echo builtin + "-n" option
		return (cmd_echo(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "cd") == 0)
		return (cmd_cd(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "pwd") == 0)
		return(cmd_pwd(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "export") == 0)
		return (cmd_export(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "unset") == 0)
		return (cmd_unset(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "env") == 0)
		return (cmd_env(cmd, ctx));
	else if (ft_strcmp(cmd->cmd, "exit") == 0)
		return (cmd_exit(cmd, ctx, shell));
    return (1);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 13:36:54 by airupert          #+#    #+#             */
/*   Updated: 2025/08/04 20:59:11 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// utils used for the builtin functions that will be used for the project

static void	free_env_array(char **env)
{
	int	i;

	if (!env)
		return ;
	i = 0;
		while (env[i])
	{
		free(env[i]);
		i++;
	}
	free(env);
}

// check if string is numeric
int	is_numeric(const char *str)
{
	if (!str || !*str)
		return (0);
	if (*str == '-' || *str == '+')
		str++;
	while (*str)
	{
		if (!(*str >= '0' && *str <= '9'))
			return (0);
		str++;
	}
	return (1);
}

// Update or add an enviornment variable
int	update_env(t_context *ctx, const char *var)
{
	char	*name_end;
	char	*name;
	size_t	name_len;
	int		i;
	int		env_count;
	char	**new_env;

	if (!ctx || !var || !ctx->env)
		return (0);
	name_end = ft_strchr(var, '=');
	if (!name_end || name_end == var)
		return (0);
	name_len = name_end - var;
	name = ft_substr(var, 0, name_len);
	if (!name)
		return (0);
	i = 0;
	while (ctx->env[i])
	{
		if (ft_strncmp(ctx->env[i], name, name_len) == 0 && ctx->env[i][name_len] == '=')
		{
			free(ctx->env[i]);
			ctx->env[i] = ft_strdup(var);
			free(name);
			if (!ctx->env[i])
				return(0);
			return (1);
		}
		i++;
	}
	// add new variable
	env_count = i;
	new_env = ft_calloc(env_count + 2, sizeof(char *));
	if (!new_env)
		return (free(name), 0);
	while (ctx->env[env_count])
		env_count++;
	i = 0;
	while (i < env_count)
	{
		new_env[i] = ft_strdup(ctx->env[i]);
		if (!new_env[i])
		{
			free_env_array(new_env);
			free(name);
			return (0);
		}
		i++;
	}
	new_env[env_count] = ft_strdup(var);
	if (!new_env[env_count])
	{
		free_env_array(new_env);
		free(name);
		return (0);
	}
	free_env_array(ctx->env);
	ctx->env = new_env;
	free(name);
	return (1);
}

// checks for valid enviornment variables
int 	is_valid_identifier(const char *str)
{
	int	i;

	if (!str || !*str)
		return (0);
	if (!ft_isalpha(str[0]) && str[0] != '_')
		return (0);
	i = 1;
	while (str[i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
			return (0);
		i++;
	}
	return (1);
}

// remove an enviornment variable
void	remove_env(t_context *ctx, const char *name)
{
	int	i;
	int found = -1;
	size_t name_len;
	
	if (!name || !*name || !ctx || !ctx->env)
		return ;
	name_len = ft_strlen(name);
	i = 0;
	while (ctx->env[i])
	{
		if (ft_strncmp(ctx->env[i], name, name_len) == 0 && (ctx->env[i][name_len] == '=' || ctx->env[i][name_len] == '\0'))
		{
			found = i;
			break;
		}
		i++;
	}
	if (found == -1)
		return ; // variable not found, no error
	free(ctx->env[found]);
	while (ctx->env[found + 1])
	{
		ctx->env[found] = ctx->env[found + 1];
		found++;
	}
	ctx->env[found] = NULL;
}

// helper to retrieve the value of an env variable
char	*get_env_value(char **env, const char *name)
{
	int		i;
	size_t	name_len;

	if (!env || !name)
		return (NULL);
	name_len = ft_strlen(name);
	i = 0;
	while (env[i])
	{
		if (ft_strncmp(env[i], name, name_len) == 0 && env[i][name_len] == '=')
			return (env[i] + name_len + 1);
		i++;
	}
	return (NULL);
}
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 17:28:32 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/04 21:03:16 by jcouto           ###   ########.fr       */
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
    if (!tokens)
    {
		ctx.exit_status = 2; //patches segfault on unclosed quotes
		return;
    }
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:20:17 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/22 20:17:48 by airupert         ###   ########.fr       */
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
Command *parse_command_args(Command *cmd, int *arg_count, t_context *ctx)
{
    cmd->args = parse_args(arg_count, ctx);
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
Command *parse_command(t_context *ctx)
{
    Command *cmd;
    int arg_count;
    cmd = parse_command_init();
    if (!cmd)
        return (NULL);
    cmd = parse_command_args(cmd, &arg_count, ctx);
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_list.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/22 19:31:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/31 20:40:39 by jcouto           ###   ########.fr       */
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
    if (!list)
        return (NULL);
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/16 13:19:50 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/31 20:38:32 by jcouto           ###   ########.fr       */
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
/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shell.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/14 14:25:06 by jcouto            #+#    #+#             */
/*   Updated: 2025/07/02 14:53:20 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// Signal handler for SIGINT (Ctrl+C)
void handle_sigint(int sig)
{
    g_signal = sig;
    write(STDOUT_FILENO, "\n", 1);
    rl_on_new_line();
    rl_replace_line("", 0);
    rl_redisplay();
}
// Signal handler for SIGQUIT (Ctrl+\)
void handle_sigquit(int sig)
{
    g_signal = sig;
    (void)sig;
}
// Initialize shell environment
int init_shell(t_shell *shell, char **envp)
{
    int i;

    shell->exit_flag = 0;
    shell->exit_status = 0;
    i = 0;
    while (envp[i])
        i++;
    shell->env = ft_calloc(i + 1, sizeof(char *));
    if (!shell->env)
        return (-1);
//    i = 0;
    if (copy_env(shell, envp, i) != 0)
        return (-1);
    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);
    return (0);
}
//Copy environment variables
int copy_env(t_shell *shell, char **envp, int env_count)
{
    int i = 0;
    while (i < env_count)
    {
        shell->env[i] = ft_strdup(envp[i]);
        if (!shell->env[i])
        {
            printf("minishell: strdup: cannot allocate memory\n");
            while (i > 0)
                free(shell->env[--i]);
            free(shell->env);
            return (-1);
        }
        i++;
    }
    shell->env[i] = NULL;
    return (0);
}
// Free shell resources
void cleanup_shell(t_shell *shell)
{
    int i;

    if (shell && shell->env)
    {
        i = 0;
        while (shell->env[i])
        {
            free(shell->env[i]);
            i++;
        }
        free(shell->env);
        shell->env = NULL;
    }
}
