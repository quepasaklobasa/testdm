#include "../../inc/minishell.h"

// Check if character at position is escaped by backslash
static int is_escaped(const char *str, int pos)
{
    int backslashes = 0;
    int i = pos - 1;
    
    while (i >= 0 && str[i] == '\\')
    {
        backslashes++;
        i--;
    }
    return (backslashes % 2 == 1);
}

// Check if we're inside single quotes at given position
static int in_single_quotes(const char *str, int pos)
{
    int in_sq = 0;
    int i = 0;
    
    while (i < pos)
    {
        if (str[i] == '\'' && !is_escaped(str, i))
            in_sq = !in_sq;
        i++;
    }
    return (in_sq);
}

// Check if we're inside double quotes at given position
static int in_double_quotes(const char *str, int pos)
{
    int in_dq = 0;
    int in_sq = 0;
    int i = 0;
    
    while (i < pos)
    {
        if (str[i] == '\'' && !is_escaped(str, i) && !in_dq)
            in_sq = !in_sq;
        else if (str[i] == '"' && !is_escaped(str, i) && !in_sq)
            in_dq = !in_dq;
        i++;
    }
    return (in_dq);
}

// Remove quotes from string while preserving content
char *remove_quotes(const char *str)
{
    if (!str)
        return (NULL);
        
    char *result = malloc(strlen(str) + 1);
    if (!result)
        return (NULL);
        
    int i = 0, j = 0;
    int in_sq = 0, in_dq = 0;
    
    while (str[i])
    {
        if (str[i] == '\'' && !is_escaped(str, i) && !in_dq)
        {
            in_sq = !in_sq;
            i++; // Skip the quote character
        }
        else if (str[i] == '"' && !is_escaped(str, i) && !in_sq)
        {
            in_dq = !in_dq;
            i++; // Skip the quote character
        }
        else
        {
            result[j++] = str[i++];
        }
    }
    result[j] = '\0';
    return (result);
}

// Check if quotes are properly closed
int quotes_closed(const char *str)
{
    int sq_count = 0, dq_count = 0;
    int i = 0;
    
    while (str[i])
    {
        if (str[i] == '\'' && !is_escaped(str, i) && !in_double_quotes(str, i))
            sq_count++;
        else if (str[i] == '"' && !is_escaped(str, i) && !in_single_quotes(str, i))
            dq_count++;
        i++;
    }
    return (sq_count % 2 == 0 && dq_count % 2 == 0);
}

// Enhanced lexer function with proper quote handling
static char *read_quoted_segment(const char *input, int *i, char quote_char)
{
    int start = ++(*i); // Skip opening quote
    
    while (input[*i] && input[*i] != quote_char)
        (*i)++;
        
    if (!input[*i])
    {
        write(STDERR_FILENO, "minishell: unclosed quote\n", 26);
        return (NULL);
    }
    
    char *segment = ft_substr(input, start, *i - start);
    (*i)++; // Skip closing quote
    return (segment);
}

// Improved word tokenization with proper quote handling
TokenNode *lexer_word_improved(char *input, int *i, int *count, TokenNode *tokens)
{
    char *buffer = ft_calloc(1, 1);
    if (!buffer)
        return (NULL);
    
    while (input[*i] && !is_operator_char(input[*i]) && input[*i] != ' ' && input[*i] != '\t')
    {
        if (input[*i] == '\'')
        {
            char *segment = read_quoted_segment(input, i, '\'');
            if (!segment)
            {
                free(buffer);
                return (NULL);
            }
            buffer = ft_strjoin_free(buffer, segment);
            free(segment);
        }
        else if (input[*i] == '"')
        {
            char *segment = read_quoted_segment(input, i, '"');
            if (!segment)
            {
                free(buffer);
                return (NULL);`
            }
            // Double quotes allow variable expansion
            char *expanded = expand_variables_in_string(segment, shell);
            buffer = ft_strjoin_free(buffer, expanded);
            free(segment);
            free(expanded);
        }
        else
        {
            // Regular character
            char temp[2] = {input[*i], '\0'};
            buffer = ft_strjoin_free(buffer, temp);
            (*i)++;
        }
        
        if (!buffer)
            return (NULL);
    }
    
    if (buffer[0])
    {
        Token new_token;
        new_token.type = TOKEN_WORD;
        new_token.value = buffer;
        (*count)++;
        return (append_token(tokens, new_token));
    }
    
    free(buffer);
    return (tokens);
}

// Helper function to check if character is an operator
static int is_operator_char(char c)
{
    return (c == '|' || c == '<' || c == '>');
}

// Main validation function to call before parsing
int validate_input(const char *input)
{
    if (!quotes_closed(input))
    {
        write(STDERR_FILENO, "minishell: unclosed quotes\n", 27);
        return (0);
    }
    return (1);
}

// Updated process_input function with validation
void process_input_improved(char *line, t_shell *shell)
{
    TokenNode *tokens;
    CommandList *cmd_list;

    // Validate input before processing
    if (!validate_input(line))
    {
        shell->exit_status = 2;
        return;
    }

    tokens = lexer(line);
    if (!tokens)
    {
        shell->exit_status = 2;
        return;
    }
    
    if (tokens->token.type == TOKEN_END)
    {
        free_tokens(tokens);
        return;
    }
    
    printf("Tokens:\n");
    print_tokens(tokens);
    cmd_list = parse_program(tokens, shell);
    if (cmd_list)
    {
        print_command_list(cmd_list);
        execute_command_list(cmd_list, shell);
        free_command_list(cmd_list);
    }
    else
        shell->exit_status = 1;
    free_tokens(tokens);
}