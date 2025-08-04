/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jcouto <jcouto@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 19:34:10 by jcouto            #+#    #+#             */
/*   Updated: 2025/08/04 19:04:43 by jcouto           ###   ########.fr       */
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

	if (!ctx)
	{
		write(STDERR_FILENO, "minishell: env: invalid identifier\n", 31);
		return (1);
	}
	if (cmd->args[1])
	{
		write(STDERR_FILENO, "minishell: env: no options or arguments allowed\n", 48);
		ctx->exit_status = 1;
		return (1);
	}
	i = 0;
	if (!ctx->env)
	{
		ctx->exit_status = 0;
		return (0);
	}
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

	i = 1;
	if (!cmd->args[1])
	{
		// unset with no arguments is valid in bash - just does nothing
		ctx->exit_status = 0;
		return (0);
	}
	while (cmd->args[i])
	{
	// Check if it's a valid identifier first
		if (!is_valid_identifier(cmd->args[i]))
		{
			write(STDERR_FILENO, "minishell: unset: `", 19);
			write(STDERR_FILENO, cmd->args[i], ft_strlen(cmd->args[i]));
			write(STDERR_FILENO, "': not a valid identifier\n", 26);
			error = 1;
		}
		else
		{
			// remove_env now returns 0 on success, 1 on error
			if (remove_env(ctx, cmd->args[i]) != 0)
			{
				write(STDERR_FILENO, "minishell: unset: failed to remove variable\n", 44);
				error = 1;
			}
		}
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
	{
		write(STDERR_FILENO, "minishell: export: invalid context\n", 34);
		if (ctx)
			ctx->exit_status = 1;
		return (1);
	}
	if (!cmd->args[1])
	{
		if (!ctx->env)
		{
			ctx->exit_status = 0;
			return (0);
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
	{
		cmd_pwd(cmd, ctx);
		return(ctx->exit_status);
	}
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
/*   Updated: 2025/08/04 19:02:48 by jcouto           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

// utils used for the builtin functions that will be used for the project

static void	free_str_array(char **arr, int count)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (i < count && arr[i])
	{
		free(arr[i]);
		i++;
	}
	free(arr);
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
	env_count = 0;
	while (ctx->env[env_count])
		env_count++;
	new_env = ft_calloc(env_count + 2, sizeof(char *));
	if (!new_env)
	{
		free(name);
		return (0);
	}
	i = 0;
	while (i < env_count)
	{
		new_env[i] = ft_strdup(ctx->env[i]);
		if (!new_env[i])
		{
			free_str_array(new_env, i);
			free(name);
			return (0);
		}
		i++;
	}
	new_env[env_count] = ft_strdup(var);
	if (!new_env[env_count])
	{
		free_str_array(new_env, env_count);
		free(name);
		return (0);
	}
	free_str_array(ctx->env, env_count);
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
int	remove_env(t_context *ctx, const char *name)
{
	if (!name || !*name || !ctx || !ctx->env)
		return (0);
	int	env_count = 0;
	while (ctx->env[env_count])
		env_count++;
	int found = -1;
	int	i = 0;
	while (ctx->env[i])
	{
		if (ft_strncmp(ctx->env[i], name, ft_strlen(name)) == 0 && (ctx->env[i][ft_strlen(name)] == '=' || ctx->env[i][ft_strlen(name)] == '\0'))
		{
			found = i;
			break;
		}
		i++;
	}
	if (found == -1)
		return (0); // variable not found, no error
	free(ctx->env[found]);
	i = found;
	while(i < env_count - 1)
	{
		ctx->env[i] = ctx->env[i + 1];
		i++;
	}
	ctx->env[env_count - 1] = NULL;
	return (0);
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