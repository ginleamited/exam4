#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

static int	fork_error(int *fd, int prev, int next)
{
	if (prev != -1)
		close(prev);
	if (next)
	{
		close(fd[0]);
		close(fd[1]);
	}
	return (1);
}

static void	child(int *fd, int prev, int next)
{
	if (prev != -1)
	{
		dup2(prev, STDIN_FILENO);
		close(prev);
	}
	if (next)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
	}
}

static int	pipe_and_fork(int *fd, char *cmd[], int *prev, int i)
{
	pid_t	pid;

	if (cmd[i + 1] && pipe(fd) == -1)
	{
		if (*prev != -1)
			close(*prev);
		return (-1);
	}
	pid = fork();
	if (pid == -1)
		return (fork_error(fd, *prev, cmd[i + 1] != NULL));
	if (pid == 0)
	{
		child(fd, *prev, cmd[i + 1] != NULL);
		execvp(&cmd[i][0], &cmd[i]);
		exit(127);
	}
	return (0);
}

static void	cleanup(int *fd, int *prev, int next)
{
	if (*prev != -1)
		close(*prev);
	if (next)
	{
		close(fd[1]);
		*prev = fd[0];
	}
}

int	picoshell(char *cmd[])
{
	int	i;
	int	fd[2];
	int	prev;
	int	result;

	if (!cmd || !cmd[0])
		return (1);
	i = 0;
	prev = -1;
	while (cmd[i])
	{
		result = pipe_and_fork(fd, cmd, &prev, i);
		if (result == -1)
			return (1);
		if (result == 1)
			return (1);
		cleanup(fd, &prev, cmd[i + 1] != NULL);
		i++;
	}
	while (wait(NULL) > 0)
		;
	return (0);
}
