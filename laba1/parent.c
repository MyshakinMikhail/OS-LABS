#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

const int BUFFER_SIZE = 256;

int main()
{
    int pipeFd[2];
    if (pipe(pipeFd) == -1)
    {
        write(STDERR_FILENO, "pipe error\n", 11);
        exit(1);
    }

    const char prompt[] = "Введите имя файла: ";
    write(STDOUT_FILENO, prompt, strlen(prompt));

    char fileName[BUFFER_SIZE];
    ssize_t n = read(STDIN_FILENO, fileName, BUFFER_SIZE - 1);
    if (n <= 0)
    {
        write(STDERR_FILENO, "read error\n", 11);
        exit(1);
    }

    if (fileName[n - 1] == '\n')
    {
        fileName[n - 1] = '\0';
    }
    else
    {
        fileName[n] = '\0';
    }

    pid_t pid = fork(); // вернет id процесса, если 0 - дочерний, > 0 - родительский
    if (pid < 0)
    {
        write(STDERR_FILENO, "fork error\n", 11);
        exit(1);
    }

    if (pid == 0)
    {
        close(pipeFd[0]);

        int fileFd = open(fileName, O_RDONLY);
        if (fileFd < 0)
        {
            char msg[256];
            snprintf(msg, sizeof(msg), "Ошибка открытия файла '%s'\n", fileName);
            write(STDERR_FILENO, msg, strlen(msg));
            exit(1);
        }

        if (dup2(fileFd, STDIN_FILENO) == -1)
        {
            write(STDERR_FILENO, "dup2 stdin error\n", 17);
            exit(1);
        }
        close(fileFd);

        if (dup2(pipeFd[1], STDOUT_FILENO) == -1)
        {
            write(STDERR_FILENO, "dup2 stdout error\n", 18);
            exit(1);
        }
        close(pipeFd[1]);

        execl("./child", "child", NULL);
        write(STDERR_FILENO, "execl failed\n", 14);
        exit(1);
    }
    else
    {
        close(pipeFd[1]);

        char buffer[BUFFER_SIZE];
        while ((n = read(pipeFd[0], buffer, sizeof(buffer))) > 0)
        {
            write(STDOUT_FILENO, buffer, n);
        }
        if (n < 0)
        {
            write(STDERR_FILENO, "read from pipe error\n", 21);
        }
        close(pipeFd[0]);
        wait(NULL);
    }

    return 0;
}
