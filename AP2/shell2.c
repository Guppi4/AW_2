#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <dirent.h>
#include <signal.h>

int flag = 0;
int i = 0;
void handler_function()
{
    printf("You typed Control-C!\n");
    return;
}
int main()
{
    char command[1024];
    char *token;
    char *prev_token;
    char prev[1024], prev2[1024];
    char *outfile;
    char *myprompt = "hello: ";
    int i, fd, amper, redirect, retid, status, piping, append = 0, read, prev_i = 0, num_pips = 0, stderr_my = 0;
    char *argv[10][10];
    char **argv_pipe[10];
    int fildes[10];
    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = handler_function;
    sigaction(SIGINT, &sigIntHandler, NULL);
    while (1)
    {
        if (flag == 1)
        {
            flag = 0;
            continue;
        }
        piping = 0;
        read = 0;
        append = 0;
        num_pips = 0;
        prev[0] = '\0';
        strcpy(prev, prev2);
        prev[strlen(prev)] = '\0';
        prev2[0] = '\0';
        printf("%s", myprompt);

        fgets(command, 1024, stdin);

        command[strlen(command) - 1] = '\0';
        /* parse command line */
        i = 0;
        int len = 0;
        int t = 0;
        token = strtok(command, " ");
        int i_ = 0;

        while (token != NULL)
        {
            if (!strcmp(token, "|"))
            {
                piping = 1;
                argv[num_pips][i] = NULL;
                num_pips += 1;
                if (num_pips == 1)
                {
                    /* code */
                    i_ = i;
                }

                strcat(prev2, token);
                strcat(prev2, " ");
                token = strtok(NULL, " ");

                i = 0;
            }
            t++;
            len += strlen(token);
            if (!strcmp(token, "!!"))
            {
                prev_token = strtok(prev, " ");
                while (prev_token != NULL)
                {
                    if (!strcmp(prev_token, "|"))
                    {
                        piping = 1;
                        argv[num_pips][i] = NULL;
                        num_pips += 1;
                        i = 0;
                        strcat(prev2, prev_token);
                        strcat(prev2, " ");
                        prev_token = strtok(NULL, " ");
                    }
                    else
                    {

                        argv[num_pips][i] = prev_token;
                        strcat(prev2, prev_token);
                        strcat(prev2, " ");
                        prev_token = strtok(NULL, " ");

                        i++;
                    }
                }
                token = strtok(command + len + t, " ");

                continue;
            }

            strcat(prev2, token);
            strcat(prev2, " ");

            argv[num_pips][i] = token;
            token = strtok(NULL, " ");

            i++;
        }
        if (i_ == 0)
        {
            i_ = i;
        }

        argv[num_pips][i] = NULL;

        prev2[strlen(prev2)] = '\0';

        /* Is command empty */
        if (argv[0][0] == NULL)
            continue;
        if (!strcmp(argv[0][0], "quit"))
        {
            exit(0);
        }
        if (!strcmp(argv[0][0], "echo"))
        {

            for (size_t j = 1; j < i_; j++)
            {
                //add if argv[0][j] == $ then  getvenv
            
               
                    char* env = getenv(argv[0][j]);
                     printf("%s ghgf", env);
                    if (env != NULL)
                    {
                       char * c=getenv(argv[0][j] + 1);
                        printf("%s ghgf", env);
                    }
                   

                if (!strcmp(argv[0][j], "$?"))
                {
                    printf("%d ", status);
                }
                else
                {
                    printf("%s ", argv[0][j]);
                }
            }
            printf("\n");
            continue;
        }
        if (!strcmp(argv[0][0], "cd") && i_ == 2)
        {
            chdir(argv[0][1]);
            continue;
        }

        if (!strcmp(argv[0][0], "prompt") && !strcmp(argv[0][1], "=") && i_ == 3)
        {
            strcat(argv[0][2], ": ");
            myprompt = argv[0][2];
            continue;
        }

        /* Does command line end with & */
        if (!strcmp(argv[0][i_ - 1], "&"))
        {
            amper = 1;
            argv[0][i_ - 1] = NULL;
        }
        else
            amper = 0;
        if (i_ > 1 && !strcmp(argv[0][i_ - 2], ">"))
        {
            redirect = 1;
            argv[0][i_ - 2] = NULL;
            outfile = argv[0][i_ - 1];
        }
        else if (i_ > 1 && !strcmp(argv[0][i_ - 2], ">>"))
        {
            redirect = 1;
            append = 1;
            argv[0][i_ - 2] = NULL;
            outfile = argv[0][i_ - 1];
        }

        else if (i_ > 1 && !strcmp(argv[0][i_ - 2], "<"))
        {
            read = 1;
            redirect = 1;
            argv[0][i_ - 2] = NULL;
            outfile = argv[0][i_ - 1];
        }
        // add  setenv from $person = "John"
        else if (i_ > 1 && !strcmp(argv[0][i_ - 2], "="))
        {
            setenv(argv[0][i_ - 3], argv[0][i_ - 1], 1);
            continue;
        }

        else if (i_ > 1 && !strcmp(argv[0][i_ - 2], "2>"))
        {
            stderr_my = 1;
            redirect = 1;
            argv[0][i_ - 2] = NULL;
            outfile = argv[0][i_ - 1];
        }

        else
            redirect = 0;
        /* for commands not part of the shell command language */

        if (fork() == 0)
        {
            /* redirection of IO ? */
            if (redirect)
            {
                if (stderr_my)
                {
                    int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                    if (fd < 0)
                    {
                        perror("Failed to open mylog");
                        return 1;
                    }

                    // Redirect the error output to the file
                    dup2(fd, STDERR_FILENO);
                    close(fd);

                    // Run the ls command with the -l flag and list the file nofile
                    execlp("ls", "ls", "-l", "nofile", NULL);

                    // If execlp returns, it means an error occurred
                    perror("execlp failed");
                    continue;
                }
                if (append)
                {
                    fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0666);
                    close(STDOUT_FILENO);
                }
                else if (read)
                {
                    fd = open(outfile, O_RDONLY | O_CREAT, 0666);
                    close(STDIN_FILENO);
                }

                else
                {
                    fd = creat(outfile, 0660);
                    close(STDOUT_FILENO);
                }
                dup(fd);
                close(fd);

                /* stdout is now redirected */
            }
            if (piping)
            {

                pipe(fildes);

                if (fork() == 0)
                {

                    for (int j = 0; j < num_pips; j++)
                    {
                        close(STDOUT_FILENO);

                        dup(fildes[num_pips - j]);
                        for (int k = j; k >= 0; k--)
                        {

                            close(fildes[k]);
                        }

                        // close(fildes[j]);

                        execvp(argv[j][0], argv[j]);
                        /* code */
                    }
                }
                close(STDIN_FILENO);
                dup(fildes[0]);
                for (size_t x = 0; x <= num_pips; x++)
                {
                    /* code */
                    close(fildes[x]);
                }

                execvp(argv[num_pips][0], argv[num_pips]);
            }
            else
                execvp(argv[0][0], argv[0]);
        }
        /* parent continues here */
        if (amper == 0)
            retid = wait(&status);
    }
}

