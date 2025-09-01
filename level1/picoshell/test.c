#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Include the picoshell function
int picoshell(char **cmds[])
{
    int i = 0;
    int fd[2];
    int in_fd = 0;   // stdin par défaut (0)
    int ret = 0;
    pid_t pid;
    int status;

    while (cmds[i])
    {
        if (cmds[i + 1]) // Si ce n'est pas la dernière commande
        {
            if (pipe(fd) == -1)
                return 1;
        }
        else
        {
            fd[0] = -1;
            fd[1] = -1;
        }

        pid = fork();
        if (pid < 0)
        {
            if (fd[0] != -1)
                close(fd[0]);
            if (fd[1] != -1)
                close(fd[1]);
            if (in_fd != 0)
                close(in_fd);
            return 1;
        }
        if (pid == 0) // Enfant
        {
            if (in_fd != 0)
            {
                if (dup2(in_fd, 0) == -1)
                    exit(1);
                close(in_fd);
            }
            if (fd[1] != -1) // si ce n'est pas le dernier
            {
                if (dup2(fd[1], 1) == -1)
                    exit(1);
                close(fd[1]);
                close(fd[0]);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1); // execvp échoué
        }
        else // Parent
        {
            if (in_fd != 0)
                close(in_fd);
            if (fd[1] != -1)
                close(fd[1]);
            in_fd = fd[0];
            i++;
        }
    }

    while (wait(&status) > 0)
    {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            ret = 1;
        else if (!WIFEXITED(status))
            ret = 1;
    }

    return ret;
}

// Test function
int main()
{
    int result;
    
    printf("=== Test 1: Simple command (ls) ===\n");
    char *cmd1[] = {"ls", NULL};
    char **test1[] = {cmd1, NULL};
    result = picoshell(test1);
    printf("Return value: %d\n\n", result);
    
    printf("=== Test 2: Pipeline - ls | grep picoshell ===\n");
    char *cmd2a[] = {"ls", NULL};
    char *cmd2b[] = {"grep", "picoshell", NULL};
    char **test2[] = {cmd2a, cmd2b, NULL};
    result = picoshell(test2);
    printf("Return value: %d\n\n", result);
    
    printf("=== Test 3: Pipeline - echo hello | cat ===\n");
    char *cmd3a[] = {"echo", "hello", NULL};
    char *cmd3b[] = {"cat", NULL};
    char **test3[] = {cmd3a, cmd3b, NULL};
    result = picoshell(test3);
    printf("Return value: %d\n\n", result);
    
    printf("=== Test 4: Three command pipeline - echo 'squalala' | cat | sed 's/a/b/g' ===\n");
    char *cmd4a[] = {"echo", "squalala", NULL};
    char *cmd4b[] = {"cat", NULL};
    char *cmd4c[] = {"sed", "s/a/b/g", NULL};
    char **test4[] = {cmd4a, cmd4b, cmd4c, NULL};
    result = picoshell(test4);
    printf("Return value: %d\n\n", result);
    
    printf("=== Test 5: Pipeline with file listing - ls | wc -l ===\n");
    char *cmd5a[] = {"ls", NULL};
    char *cmd5b[] = {"wc", "-l", NULL};
    char **test5[] = {cmd5a, cmd5b, NULL};
    result = picoshell(test5);
    printf("Return value: %d\n\n", result);
    
    printf("=== Test 6: Error case - invalid command ===\n");
    char *cmd6[] = {"nonexistentcommand", NULL};
    char **test6[] = {cmd6, NULL};
    result = picoshell(test6);
    printf("Return value: %d (should be 1 for error)\n\n", result);
    
    return 0;
}
