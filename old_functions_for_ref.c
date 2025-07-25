// Execute external command
int exec_external(Command *cmd, t_context *ctx)
{
    char *path;
    pid_t pid;
    int status;
    path = get_command_path(cmd->cmd);
    if (!path)
    {
        ctx->exit_status = 1;
        return (1);
    }
    pid = fork();
    if (pid == 0)
    {
        execve(path, cmd->args, ctx->env);
        write(2, "minishell: command not found\n", 29);
        exit(127);
    }
    else if (pid > 0)
    {
        cmd->pid = pid;
        waitpid(pid, &status, 0);
        ctx->exit_status = WEXITSTATUS(status);
    }
    else
    {
        write(2, "minishell: fork: cannot create process\n", 38);
        ctx->exit_status = 1;
    }
    free(path);
    return (ctx->exit_status);
}
