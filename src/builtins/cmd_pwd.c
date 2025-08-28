/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pwd.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: airupert <airupert@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 15:28:00 by airupert          #+#    #+#             */
/*   Updated: 2025/08/28 22:03:10 by airupert         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/minishell.h"

int	cmd_pwd(Command *cmd, t_shell *shell)
{
	char	cwd[1024];

	if (getcwd(cwd, sizeof(cwd)))
	{
		write(cmd->out_fd, cwd, ft_strlen(cwd));
		write(cmd->out_fd, "\n", 1);
		shell->exit_status = 0;
	}
	else
	{
		write(STDERR_FILENO, "minishell: pwd: \
			error retrieving current directory\n", 50);
		shell->exit_status = 1;
	}
	return (shell->exit_status);
}
