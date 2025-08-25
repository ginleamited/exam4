#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

static pid_t	g_child_pid = 0;

static void	my_timeout(int sig)
{
	(void)sig;
	if (g_child_pid > 0)
		kill(g_child_pid, SIGKILL);
}

static int	setup_signal(void)
{
	struct sigaction	sa;

	sa.sa_handler = my_timeout;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) == -1)
		return (-1);
	return (0);
}

static int	child(void (*f)(void))
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}
	g_child_pid = pid;
	return (0);
}

static int	my_exit(int status, bool verbose)
{
	int	exit_code;

	exit_code = WEXITSTATUS(status);
	if (exit_code == 0)
	{
		if (verbose)
			printf("Nice function!\n");
		return (1);
	}
	else
	{
		if (verbose)
			printf("Bad function: exited with code %d\n", exit_code);
		return (0);
	}
}

static int	signal_status(int status, unsigned int timeout, bool verbose)
{
	int	sig;

	sig = WTERMSIG(status);
	if (sig == SIGKILL)
	{
		if (verbose)
			printf("Bad function: timed out after %u seconds\n", timeout);
	}
	else
	{
		if (verbose)
			printf("Bad function: %s\n", strsignal(sig));
	}
	return (0);
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	int	status;

	if (!f)
		return (-1);
	if (setup_signal() == -1)
		return (-1);
	if (child(f) == -1)
		return (-1);
	alarm(timeout);
	if (waitpid(g_child_pid, &status, 0) == -1)
	{
		alarm(0);
		g_child_pid = 0;
		return (-1);
	}
	alarm(0);
	g_child_pid = 0;
	if (WIFEXITED(status))
		return (my_exit(status, verbose));
	else if (WIFSIGNALED(status))
		return (signal_status(status, timeout, verbose));
	return (0);
}
/*
// Test functions
void	nice_function(void)
{
	// Function that does nothing and exits normally
}

void	bad_exit_function(void)
{
	exit(42);  // Exit with non-zero code
}

void	segfault_function(void)
{
	int *p = NULL;
	*p = 42;  // Segmentation fault
}

void	infinite_loop_function(void)
{
	while (1)
		;  // Infinite loop to test timeout
}

void	abort_function(void)
{
	abort();  // Abort signal
}

int main(void)
{
	int result;

	printf("=== Test 1: Nice function ===\n");
	result = sandbox(nice_function, 2, true);
	printf("Result: %d\n\n", result);

	printf("=== Test 2: Bad exit code ===\n");
	result = sandbox(bad_exit_function, 2, true);
	printf("Result: %d\n\n", result);

	printf("=== Test 3: Segmentation fault ===\n");
	result = sandbox(segfault_function, 2, true);
	printf("Result: %d\n\n", result);

	printf("=== Test 4: Timeout (infinite loop) ===\n");
	result = sandbox(infinite_loop_function, 2, true);
	printf("Result: %d\n\n", result);

	printf("=== Test 5: Abort signal ===\n");
	result = sandbox(abort_function, 2, true);
	printf("Result: %d\n\n", result);

	printf("=== Test 6: NULL function (error) ===\n");
	result = sandbox(NULL, 2, true);
	printf("Result: %d\n\n", result);

	printf("=== Test 7: Verbose off ===\n");
	result = sandbox(bad_exit_function, 2, false);
	printf("Result: %d (no message should appear above)\n\n", result);

	return (0);
}
*/