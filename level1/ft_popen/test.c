#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

// Include the ft_popen function
int	ft_popen(const char *file, char *const argv[], char type)
{
	if (!file || !argv || (type != 'r' && type != 'w'))
		return -1;

	int fd[2];

	pipe(fd);
	if (type == 'r')
	{
		if (fork() == 0)
		{
			dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);
			close(fd[1]);
			execvp(file, argv);
			exit (-1);
		}
		close(fd[1]);
		return (fd[0]);
	}
	if (type == 'w')
	{
		if (fork() == 0)
		{
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
			close(fd[1]);
			execvp(file, argv);
			exit (-1);
		}
		close(fd[0]);
		return (fd[1]);
	}
	return -1;
}

int main()
{
	printf("=== Testing ft_popen with 'r' (reading from command output) ===\n");
	
	// Test 1: Read from 'ls' command
	int fd = ft_popen("ls", (char *const []){"ls", "-l", NULL}, 'r');
	if (fd == -1)
	{
		printf("Error: ft_popen failed\n");
		return 1;
	}
	
	printf("Reading from 'ls -l' command:\n");
	char buffer[1024];
	ssize_t bytes_read;
	while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0)
	{
		buffer[bytes_read] = '\0';
		printf("%s", buffer);
	}
	close(fd);
	wait(NULL); // Wait for child process to finish
	
	printf("\n=== Testing ft_popen with 'w' (writing to command input) ===\n");
	
	// Test 2: Write to 'cat' command (it will echo what we write)
	fd = ft_popen("cat", (char *const []){"cat", NULL}, 'w');
	if (fd == -1)
	{
		printf("Error: ft_popen failed\n");
		return 1;
	}
	
	printf("Writing 'Hello, World!' to cat command:\n");
	write(fd, "Hello, World!\n", 14);
	write(fd, "This is a test\n", 15);
	close(fd);
	wait(NULL); // Wait for child process to finish
	
	printf("\n=== Testing error cases ===\n");
	
	// Test 3: Invalid parameters
	fd = ft_popen(NULL, (char *const []){"ls", NULL}, 'r');
	printf("ft_popen with NULL file: %d (should be -1)\n", fd);
	
	fd = ft_popen("ls", NULL, 'r');
	printf("ft_popen with NULL argv: %d (should be -1)\n", fd);
	
	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'x');
	printf("ft_popen with invalid type: %d (should be -1)\n", fd);
	
	return 0;
}
