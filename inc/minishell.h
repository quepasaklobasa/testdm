/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 17:28:28 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/12 10:56:47 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <readline/readline.h>     // readline, add_history, rl_*
# include <readline/history.h>      // history management
# include <stdio.h>                 // printf, perror
# include <stdlib.h>                // malloc, free, exit, getenv
# include <unistd.h>                // write, access, read, close, fork, execve, getcwd, chdir, dup, dup2, isatty, ttyname, ttyslot
# include <fcntl.h>                 // open, O_* flags
# include <sys/types.h>             // pid_t, DIR, stat
# include <sys/wait.h>              // wait, waitpid, wait3, wait4
# include <signal.h>                // signal, sigaction, sigemptyset, sigaddset, kill
# include <sys/stat.h>              // stat, lstat, fstat
# include <dirent.h>                // opendir, readdir, closedir
# include <string.h>                // strerror
# include <sys/ioctl.h>             // ioctl
# include <termios.h>               // tcsetattr, tcgetattr
# include <curses.h>                // tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
# include <term.h>                  // alternate curses interface (required on some systems)
# include "../libft/libft.h"

extern volatile sig_atomic_t g_signal; // Global var for sighandling

//Structure to hold shell state
typedef struct s_shell {
    char    **env;
	int		exit_status;
    volatile sig_atomic_t exit_flag;
    int     pipefd[2];
    int     prev_fd;
} t_shell;

typedef enum { //Token types for lexer
    TOKEN_WORD,
    TOKEN_VARIABLE,
    TOKEN_PIPE,
    TOKEN_REDIRECT_IN,
    TOKEN_REDIRECT_OUT,
    TOKEN_REDIRECT_APPEND,
    TOKEN_HEREDOC,
    TOKEN_END
} TokenType;

typedef struct s_token {
    TokenType type;
    char *value;
} Token;

// Token node for linked list
typedef struct s_token_node {
    Token token;
    struct s_token_node *next;
} TokenNode;

// Parser globals
extern TokenNode *g_tokens;
extern TokenNode *g_current_token;

typedef struct s_command {
    char *cmd;              // Command name (e.g., "echo")
    char **args;            // Arguments (e.g., ["echo", "$HOME"])
    char *redirect_in;      // Input redirection file (e.g., "input.txt")
    char *redirect_out;     // Output redirection file (e.g., "file")
    char *redirect_append;  // Append redirection file (e.g., "file")
    char *heredoc_delim;    // Here_document
    int in_fd;              // Input file descriptor
    int out_fd;             // Output file descriptor
    int pid;                // Process ID
} Command;

typedef struct s_command_list {
    Command *cmd;
    struct s_command_list *next; // Next command in pipeline
} CommandList;

// src/shell/shell.c
void handle_sigint(int sig);
void handle_sigquit(int sig);
int init_shell(t_shell *shell, char **envp);
void cleanup_shell(t_shell *shell);
int copy_env(t_shell *shell, char **envp, int env_count);

// src/lexer/lexer.c
TokenNode *lexer_init(void);
TokenNode *lexer_operator(char *input, int *i, int *count, TokenNode *tokens);
TokenNode *lexer_single_quote(char *input, int *i, int *count, TokenNode *tokens);
TokenNode *lexer_double_quote(char *input, int *i, int *count, TokenNode *tokens);
TokenNode *lexer(char *input);

// src/lexer/lexer_operators.c
void lexer_operator_type(char *input, int *i, TokenType *type, int *inc);
TokenNode *lexer_operator_value(TokenNode *tokens, int count, TokenType type);

// src/lexer/lexer_super_words.c
TokenNode *lexer_word_combined(char *input, int *i, int *count, TokenNode *tokens);
int lexer_classify(char *input, int i);
TokenNode *lexer_variable(char *input, int *i, int *count, TokenNode *tokens);

// src/lexer/lexer_utils.c
void free_tokens(TokenNode *tokens);
TokenNode *append_token(TokenNode *tokens, Token token);

// src/parser/parser.c
int consume(TokenType type);
char **parse_args_alloc(int *arg_count);
char **parse_args_process(char **args, int *arg_count, t_shell *shell);
char **parse_args(int *arg_count, t_shell *shell);
int parse_redirection_type(Command *cmd, TokenType type);

// src/parser/command.c
int parse_redirection(Command *cmd);
Command *parse_command_init(void);
Command *parse_command_args(Command *cmd, int *arg_count, t_shell *shell);
Command *parse_command(t_shell *shell);
void free_command(Command *cmd);

// src/parser/command_list.c
CommandList *init_command_list(t_shell *shell);
CommandList *handle_pipe(CommandList *list, t_shell *shell); // removed token stream argument
CommandList *parse_program(TokenNode *token_stream, t_shell *shell);
CommandList *parse_command_list(t_shell *shell);
void free_command_list(CommandList *list);

// src/execution/execution.c
int execute_command_list(CommandList *cmd_list, t_shell *shell); // added t_shell proto
int execute_single_command(Command *cmd, t_shell *shell);

// src/builtins/builtin.c
int is_builtin(const char *cmd);
int exec_builtin(Command *cmd, t_shell *shell); // added t_shell proto

// src/builtins/builtin_utils.c
int		update_env(t_shell *shell, const char *var);
void	remove_env(t_shell *shell, const char *name);
int		is_numeric(const char *str);
int		is_valid_identifier(const char *str);
char    *get_env_value(char **env, const char *name);

// src/execution/external.c
char	*get_command_path(const char *cmd, t_shell *shell);
int		exec_external(Command *cmd, t_shell *shell);

// src/execution/external_utils.c
char	*ft_strtok(char *str, const char *delim);

// src/execution/utils.c
void	setup_fds(Command *cmd, t_shell *shell);

// src/main/main.c
void process_input(char *line, t_shell *shell);
int run_shell_loop(t_shell *shell);
int main(int ac, char **av, char **env);


#endif